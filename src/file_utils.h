/**
 * file_utils.h: Utility functions for walking through file system objects
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#ifndef __file_utils_h__
#define __file_utils_h__
#include <iostream>
#include <string>
#include <vector>
#include <errno.h>
#include <string.h>
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

/**
 * Abstract interface used by the process_files function below.
 */
class FileProcessor {
public:
    /**
     * Given a pathname/filename, perform some useful action on it. Return true if the action was successful.
     */
    virtual bool process_file(const string& file) = 0;
};


// Helper functions
bool list_files(string dir, vector<string>& files);
int is_directory(string filename);


/**
 * Given a list of files, invokes the action of a "processor" on each of these files in order.
 * Knows when to stop or when to recurse into subdirectories.
 * Returns true if there were no errors.
 */
/**
 * Given a list of files, invokes the action of a "processor" on each of these files in order.
 * Knows when to stop or when to recurse into subdirectories.
 * Returns true if there were no errors.
 */
template <typename String> bool process_files(vector<String> files, bool recursive, bool fail_on_error, FileProcessor* p, bool quiet = false) {
    int processed_count = 0;
    bool result = true;
    
	// Now hash files one by one
	for (int i = 0; i < files.size(); i++) {
        processed_count++;
        bool current_result;
        int is_dir = is_directory(files[i]);
        if (is_dir == -1) {
            // Error occured:
            cerr << "Error: " << strerror(errno) << endl;
            current_result = false;
        }
        else if (!recursive || (is_dir == 0))
            current_result = p->process_file(files[i]);
        else {
            // It's a valid directory and we should recurse
            vector<string> dir_contents;
            if (!list_files(files[i], dir_contents)) {
                current_result = false;
                cerr << "Error: " << strerror(errno) << endl;
            }
            else {
                current_result = process_files(dir_contents, recursive, fail_on_error, p, true);
            }
        }
        if (!current_result) {
            result = false;
            if (fail_on_error) {
        		// Stop processing on any error, if the user asked us to (otherwise we'll continue).
       			if (!quiet) {
                    if (!recursive && (processed_count < files.size())) {
            			cerr << "Note: " << (files.size() - processed_count) << " files left unprocessed." << endl;
                    }
                    else {
                        cerr << "Note: some files might have been left unprocessed." << endl;
                    }
       			}
       			break;
   		    }
        }
    }
    return result;
}


#endif
