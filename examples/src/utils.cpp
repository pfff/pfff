/**
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include "utils.h"
#include <fstream>
#include <iostream>
#include <errno.h>
#include <sys/stat.h>
#ifdef _WIN32
    #include "windows.h"
#endif
#ifdef __MINGW32__
    // Mingws uses struct _stati64 and function _stati64, in place of POSIX's stat64
    #define stat64 _stati64
#endif
#ifdef __CYGWIN__
	// struct stat64 is not used in Cygwin, just use struct stat. It's 64 bit aware.
	// http://www.cygwin.com/faq/faq.programming.html#faq.programming.stat64
	#define stat64 stat
#endif
using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;

/**
 * Flushes the system cache of a given file.
 * NB: It does not flush the hard drive device cache, hence repeated reads are still kinda faster than the first reads.
 * Nonetheless, it helps for fairer comparisons.
 * Beware: this function is Win32-specific. It simply does nothing on *nix systems.
 */
void flush_file_buffers(const char* filename) {
    #ifdef _WIN32
    HANDLE hFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        cerr << "Error: Unable to flush cache for " << filename << endl;
    }
    else {
        FlushFileBuffers(hFile); // NB: It seems that flushing is in fact done by the FILE_FLAG_NO_BUFFERING and this call does not make any difference.
        CloseHandle(hFile);
    }
    #endif
}


/**
 * Reads lines from a given file.
 */
vector<string> getlines(const char* filename) {
    ifstream in(filename);
    vector<string> result;
    while(!in.eof()) {
        string line;
        getline(in, line);
        result.push_back(line);
    }
    in.close();
    return result; 
}

/** 
 * Read file size
 */
long long file_size(const char* filename) {
    struct stat64 s;
    int result;
    for (int i = 0; i < 10; i++) { // If stat returns EAGAIN, we'll try this 10 times
        result = stat64(filename, &s);
        if (result == 0 || errno != EAGAIN) break;
    }
    if (result != 0) {
        //error_message = strerror(errno);
        return -1;
    }
    if (!S_ISREG(s.st_mode)) {
        //error_message = "Object ";
        //error_message = error_message + filename + " is not a file.";
        return -2;
    }
    else {
        return s.st_size;
    }
}
