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
#include <sys/stat.h>
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

inline const char* as_charptr(const string& s) { return s.c_str(); }
inline const char* as_charptr(const char* s) { return s; }

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
template <typename String> bool process_files(vector<String> files, bool recursive, bool no_symlinks, bool fail_on_error, FileProcessor* p, bool quiet = false) {
    int processed_count = 0;
    bool result = true;
    
    // Now hash files one by one
    for (int i = 0; i < files.size(); i++) {
        processed_count++;
        bool current_result;
        struct stat s, ls;
        int stat_error, lstat_error;
        const char* filename = as_charptr(files[i]); // We need a char* to invoke stat/lstat
        
        // Do we need to check for symlinks?
        if (no_symlinks)
            lstat_error = lstat(filename, &ls);
        else {
            lstat_error = 0;
            ls.st_mode = 0;
        }
        
        // Do we need to check whether file is a directory?
        if (recursive)
            stat_error = lstat_error || stat(filename, &s);
        else {
            stat_error = lstat_error;
            s.st_mode = 0;
        }
        
        if (stat_error != 0) {
            // Error occured when statting:
            cerr << "Error: " << strerror(errno) << endl;
            current_result = false;
        }
        else if (no_symlinks && S_ISLNK(ls.st_mode)) {
            // It's a symlink and we must ignore it
            cerr << "Note: ignoring symlink " << filename << endl;
        }
        else if (recursive && S_ISDIR(s.st_mode)) {
            // It's a valid directory and we should recurse
            vector<string> dir_contents;
            if (!list_files(files[i], dir_contents)) {
                current_result = false;
                cerr << "Error: " << strerror(errno) << endl;
            }
            else {
                current_result = process_files(dir_contents, recursive, no_symlinks, fail_on_error, p, true);
            }
        }
        else {
            // We don't care whether this is a directory or symlink, just pass it to the processor
            current_result = p->process_file(files[i]);
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
