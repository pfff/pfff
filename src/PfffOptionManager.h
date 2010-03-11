/**
 * PfffOptionManager.h: Class for managing the command-line options to the pfff tool.
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#ifndef __PfffOptionManager_h__
#define __PfffOptionManager_h__
#include <iostream>
#include <stdint.h>
#include "OptionManager.h"
#include "PfffOptions.h"
using std::ostream;

/**
 * Command-line option parser, with documentation, validation and stuff...
 */
struct PfffOptionManager: public OptionManager {
public:
	// Used to disable "exit(2)" in die_with_error. Instead, makes the
	// procedure throw a string.
	bool TEST_MODE;
		
	char* format;
	long  key;
	int   key_given;
	long  block_count;
	long  block_size;
	long  header_block_count;
	int   without_replacement;
	int   with_size;
	int   no_prefix;
	int   no_filename;

	int   help;
	long  request_cost;
	int   request_cost_given;
	int   fail_on_error;
	int   recursive;
	
	int   ftp_given;
	char* ftp_host;
	int   ftp_debug;
	//long  ftp_request_cost;
	//int   ftp_request_cost_given;
	
	string error_message;
    PfffOptions options;
	
	PfffOptionManager();
	
	/**
	 * Reads options from command line. On any failure prints error message and dies.
	 */
	void init_from_cmdline_or_die(int argc, char* argv[]);
	
	/**
	 * Dumps a long 'help' message describing all the options to a given ostream.
	 */
	void print_usage(ostream& out);

	/**
	 * Equivalent to print(message,cerr), exit(2)
	 */
	void die_with_error(const string& message);
	
	/**
	 * Returns true if options are valid. Otherwise returns false and sets the
	 * error_message field.
	 */
	bool validate();
	
	/**
	 * if (!validate()) die_with_error(error_message)
	 */
	void validate_or_die();	
};

#endif

