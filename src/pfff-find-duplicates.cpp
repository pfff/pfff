/**
 * pfff-find-duplicates.cpp: Tool for searching for dupicate files using pfff-hashing.
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include <iostream>
#include <sstream>
#include "file_utils.h"
#include "PfffBlockReader.h"
#include "PfffFtpBlockReader.h"
#include "PfffHasher.h"
#include "PfffFindDuplicatesOptionManager.h"
#include <stdlib.h>

using std::ostringstream;

/**
 * A convenience class, counting observed hashed objects and grouping duplicates
 */
template <typename HashType, typename ContentType> class DuplicateTracker {
public:
    typedef map<HashType, vector<ContentType> > duplicates_map; // Map(Hash --> Content)
	map<HashType, vector<ContentType> > groups;
	vector<HashType> non_singleton_groups;                   // Collects hashes which have multiple filenames

    void process_entry(const HashType& hash, const ContentType& content) {
		// Have we seen this hash already?
		typename map<HashType ,vector<string> >::iterator f = groups.find(hash);
		if (f != groups.end()) { 
            // We have already seen this hash!
            // Append to the list
            f->second.push_back(content);
            // If the list is now of length "2", register the group in the "non_singleton_groups" registry
            if (f->second.size() == 2) {
                non_singleton_groups.push_back(hash);
            }
        }
        else {
            // Create a new entry
            groups[hash].push_back(content);
        }            
    }
    
    /**
     * List duplicate groups
     */
    void output_duplicates(ostream& out) {
        out << "# Found " << non_singleton_groups.size() << " duplicate groups" << endl;
        long current_group_index = 0;
        for (typename vector<HashType>::iterator i = non_singleton_groups.begin(); i != non_singleton_groups.end(); i++) {
            current_group_index++;
            vector<ContentType> elements = groups[*i];
            for (typename vector<ContentType>::iterator j = elements.begin(); j != elements.end(); j++)
                out << current_group_index << '\t' << *j << endl;
        }
    }
};

/**
 * A convenience class, wrapping the main application logic (implementing the FileProcessor interface)
 */
class PfffFindDuplicatesAppEngine: public FileProcessor {
public:
    PfffFindDuplicatesOptionManager option_manager;
    FtpClientSocket* ftp_connection;
    PfffHasher* hasher;
    DuplicateTracker<string, string> dup_tracker;
    
    PfffFindDuplicatesAppEngine(): ftp_connection(NULL), hasher(NULL) {}
    
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
    	dup_tracker.output_duplicates(cout);
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
            // TODO: Technically this place can be streamlined so that we don't need the ostringstream but instead work directly on hashed bytes.
            ostringstream out;
			hasher->hash(out, input_file);
			dup_tracker.process_entry(out.str(), filename);
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
    PfffFindDuplicatesAppEngine* engine = new PfffFindDuplicatesAppEngine();
    engine->init(argc, argv);
    bool recursive = engine->option_manager.recursive && !engine->option_manager.ftp_given;
    bool success = process_files(engine->option_manager.parameters, recursive, engine->option_manager.no_symlinks, engine->option_manager.fail_on_error, engine);
    engine->quit();
    delete engine;
	return success ? 0: 1;
}
