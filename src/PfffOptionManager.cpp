/**
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include "PfffOptionManager.h"
#include <cstdlib>
#include <getopt.h> 
#include <string.h>
#include <vector>

#define quote_(x) #x        // Used to pass "defined" values into string literals, see http://en.wikipedia.org/w/index.php?title=C_preprocessor&oldid=346873133#Indirectly_quoting_macro_arguments
#define quote(x) quote_(x)

using std::cerr;
using std::cout;
using std::endl;
using std::vector;

// ------------ PfffOptionManager ---------------
PfffOptionManager::PfffOptionManager(): OptionManager(), TEST_MODE(false) {
    add_group("Basic Algorithm Options");
        add_parameterized("key", 'k', &key_given, new BoundedLongIntOption(&key, PFO_KEY_MIN, PFO_KEY_MAX, 0), "<num>",
            "Randomization key, given as a positive integer between\n"
            quote(PFO_KEY_MIN) " and " quote(PFO_KEY_MAX) ". No default, must be specified.");
        add_parameterized("with-header", 'H', NULL, new BoundedLongIntOption(&header_block_count, PFO_HBC_MIN, PFO_HBC_MAX, PFO_HBC_DEFAULT), "<num>", 
            "Include <num> first blocks from the file verbatim.\n"
            "(the remaining part of the file will be hashed as\n"
            "usually). <num> <= " quote(PFO_HBC_MAX) ".");
        add_unparameterized("with-size", 'S', &with_size, 
            "Include the size of the file in bytes into the hash.\n");
    add_group("Output Options");
        add_parameterized("format", 'f', NULL, new CharPtrOption(&format, "poly1305aes"), "<format>", 
            "Specify the format of the output hash. Currently\n"
            "supported values for <format> are the following:\n"
            "  poly1305aes - sampled blocks are further hashed using\n"
            "          an efficient Poly1305 AES MAC hash to a 128\n"
            "          bit number and encoded in hex.\n"
            "  md5   - sampled blocks are further hashed using\n"
            "          standard md5\n"
            "  csv   - sampled blocks are output as comma-separated\n"
            "          hex-encoded values\n"
            "  debug - undocumented secret option\n"
            "Default is 'poly1305aes'.");
        add_unparameterized("no-prefix", 'b', &no_prefix, 
            "Do not prefix the output fingerprint with the encoded \n"
            "parameter information. Makes output smaller by some\n"
            "bytes, but the use of pfff-verify becomes more\n"
            "complicated. Default is to include the prefix.\n"
            "By the way, 'b' stands for 'bare'.");
        add_unparameterized("no-filename", 'B', &no_filename,
            "Do not output the filename(s), just the fingerprints.");
        add_unparameterized("help", 'h', &help, 
            "Output this help message to stdout.");
    add_group("Advanced Options");
        add_parameterized("block-count", 'n', NULL, new BoundedLongIntOption(&block_count, PFO_BC_MIN, PFO_BC_MAX, PFO_BC_DEFAULT), "<num>",
            "Number of blocks to sample. Default is " quote(PFO_BC_DEFAULT) ".\n"
            "Maximum is " quote(PFO_BC_MAX) ".");
        add_parameterized("block-size", 's', NULL, new BoundedLongIntOption(&block_size, PFO_BS_MIN, PFO_BS_MAX, PFO_BS_DEFAULT), "<num>",
            "Size of each block in bytes. Default is " quote(PFO_BS_DEFAULT) ".\n"
            "Maximum is " quote(PFO_BS_MAX) ".");
        add_unparameterized("without-replacement", 'w', &without_replacement,
            "Sample without replacement (default is with\n"
            "replacement, it's faster).");
        add_parameterized("request-cost", 'c', &request_cost_given, new PositiveLongIntOption(&request_cost, 0), "<num>",
            "Cost of making a separate data read request, in bytes.\n"
            "It specifies that whenever the algorithm must request\n"
            "two blocks that are separated by a gap less than the\n"
            "request cost, the two blocks will be requested in a\n"
            "single read. Defaults are: 0 for local files and\n"
            "1024000 for FTP. Do use this parameter if you access\n"
            "files over NFS.");
        add_unparameterized("fail-on-error", 'E', &fail_on_error, 
            "Fail on any error. By default, when hashing several\n"
            "files, pfff will skip errors on individual files and\n"
            "proceed to process all files. When this option is\n"
            "given, pfff will break as soon as any error occurs.");
        add_unparameterized("recursive", 'R', &recursive, 
            "Recurse into subdirectories. Ignored for FTP access.");
        add_unparameterized("no-symlinks", 'L', &no_symlinks,
            "Ignore symlinks.");
    add_group("Experimental Options");
        add_parameterized("ftp-host", 'F', &ftp_given, new CharPtrOption(&ftp_host, ""), "<hostname>",
            "Interpret all files as absolute paths on the\n"
            "given FTP host.");
        add_unparameterized("ftp-debug", 'G', &ftp_debug, "Output complete FTP protocol log.");
        /*add_parameterized("ftp-request-cost", 'c', &ftp_request_cost_given, new PositiveLongIntOption(&ftp_request_cost, 1024000), "<num>",
            "Cost of making a separate data read request, in bytes.\n"
            "It specifies that whenever the algorithm must request\n"
            "two blocks that are separated by a gap less than the\n"
            "request cost, the two blocks will be requested in a\n"
            "single read. Default is 1024000.");*/
}

/**
 * Reads options from command line. On any failure prints error message and dies.
 */
void PfffOptionManager::init_from_cmdline_or_die(int argc, char* const argv[]) {
    if (!read_from_cmdline(argc, argv)) die_with_error("");
    validate_or_die();
}

/**
 * Dumps a long 'help' message describing all the options to a given ostream.
 */
void PfffOptionManager::print_usage(ostream& out) {
    const char* USAGE = 
    "Probabilistic Fast File Fingerprinting (PFFF) - A tool for computing file hashes\n"
    "using a fast probabilistic method.\n"
    "\n"
    "Usage: pfff [options] <file1> <file2> ...\n"
    "\n"
    "Parameters:\n"
    "    <file1>, <file2>, ...: paths or names of the files to be fingerprinted.\n"
    "\n";
    out << USAGE;
    print_option_help(out);
}

/**
 * Equivalent to print(message,cerr), exit(2)
 */
void PfffOptionManager::die_with_error(const string& message) {
    if (TEST_MODE) throw message;
    *cerr << message << endl;
    *cerr << "Run the program with the --help option to get usage information." << endl;
    #ifdef DEBUG
    while(1) if ('\n' == getchar()) break;
    #endif
    exit(2);
}

bool PfffOptionManager::validate() {
    if (help) return true;
    try {
    	if (parameters.size() == 0)
    		throw (char*)"Error: No files to process.";
    	if (!key_given) 
    		throw (char*)"Error: You must provide a value for the key.";
    	
    	// If we're using ftp and haven't specified request_cost, set a 
    	// reasonable default.
    	if (!request_cost_given && ftp_given) request_cost = 1024000; 
    	
    	// Now fill in the options structure
        pfff_options_init(&options, key);
    	if (strcmp(format, "poly1305aes") == 0)
    		options.output_format = PFO_OF_POLY1305AES;
    	else if (strcmp(format, "md5") == 0) 
    		options.output_format = PFO_OF_MD5;
    	else if (strcmp(format, "csv") == 0) 
    		options.output_format = PFO_OF_CSV;
    	else if (strcmp(format, "debug") == 0)
    		options.output_format = PFO_OF_DEBUG;
    	else
    		throw (char*)"Error: Unrecognized output format.";
        options.block_count = block_count;
        options.block_size = block_size;
        options.header_block_count = header_block_count;
        options.without_replacement = without_replacement;
        options.with_size = with_size;
        options.no_prefix = no_prefix;
        options.no_filename = no_filename;
        
        char* errmsg;
        if (pfff_options_validate(&options, &errmsg)) return true;
        else throw errmsg;
    }
    catch(char* msg) {
        error_message = string(msg);
        return false;
    }
}

/**
 * if (!validate()) die_with_error(error_message)
 */
void PfffOptionManager::validate_or_die() {
    if (!validate()) die_with_error(error_message);
}


