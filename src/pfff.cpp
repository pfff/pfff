/**
 * pfff.cpp: Main file of the pfff hashing tool.
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include <iostream>
#include "file_utils.h"
#include "PfffBlockReader.h"
#include "PfffFtpBlockReader.h"
#include "PfffHasher.h"
#include "PfffOptionManager.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * A convenience class, wrapping the main application logic (implementing the FileProcessor interface)
 */
class PfffAppEngine: public FileProcessor {
public:
    PfffOptionManager option_manager;
    FtpClientSocket* ftp_connection;
    PfffHasher* hasher;

    PfffAppEngine(): ftp_connection(NULL), hasher(NULL) {}
    
    /**
     * Should be called to initialize application.
     * This function may decide to call exit(..) if it feels like it
     * (e.g. initialization fails or the user requested for help and there is nothing left to do).
     */
    void init(int argc, char* argv[]) {
    	// Read command line options
    	option_manager.init_from_cmdline_or_die(argc, argv);

  		// Did the user ask for help?
    	if (option_manager.help) {
    		option_manager.print_usage(cout);
    		exit(0);
    	}
    	
    	// Initialize ftp connection, if necessary
    	if (option_manager.ftp_given) {
    		if (option_manager.ftp_debug) Socket::DEBUG = true;
    		ftp_connection = new FtpClientSocket(option_manager.ftp_host);
    		string response = ftp_connection->AnonymousLogin();
    		if (response[0] != '2') {
    			cerr << "FTP login failed" << endl;
    			delete ftp_connection;
    			exit(1);
    		}
    		response = ftp_connection->SendCommand("TYPE I");
    		if (response[0] != '2') {
    			cerr << "FTP operation TYPE I failed" << endl;
    			delete ftp_connection;
    			exit(1);
    		}			
    	}
    	
    	// Initialize hasher
    	hasher = new PfffHasher(&option_manager.options);    	
    }
    
    void quit() {
    	delete hasher;
    	if (option_manager.ftp_given) delete ftp_connection;
    }
    
    /**
     * Returns false on error, true on success.
     */
    bool process_file(const string& filename) {
    	bool result = true;
    	BlockReader* input_file;
    	if (option_manager.ftp_given) 
    		input_file = new FtpBlockReader(ftp_connection, filename.c_str());
    	else 
    		input_file = new LocalFileBlockReader(filename.c_str());
    		
    	if (option_manager.request_cost > 0) input_file = new BufferingBlockReader(input_file, option_manager.request_cost);
    	
    	try {
    		hasher->hash(cout, input_file);
    		cout << endl;
    	}
    	catch(pfff_exception& e) {
    		cerr << "Error: " << e.what() << endl;
    		result = false;
    	}
    	delete input_file;
    	return result;
    }
};

int main(int argc, char* argv[]) {
    PfffAppEngine* engine = new PfffAppEngine();
    engine->init(argc, argv);
    bool recursive = engine->option_manager.recursive && !engine->option_manager.ftp_given;
    bool success = process_files(engine->option_manager.parameters, recursive, engine->option_manager.no_symlinks, engine->option_manager.fail_on_error, engine);
    engine->quit();
    delete engine;
	return success ? 0: 1;
}
