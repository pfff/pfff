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

#include "md5.h"
using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;

namespace FileUtils {

/**
 * Flushes the system cache of a given file.
 * NB: It does not flush the hard drive device cache, hence repeated reads are still kinda faster than the first reads.
 * Nonetheless, it helps for fairer comparisons.
 * Beware: this function is Win32-specific. It simply does nothing on *nix systems.
 */
void flush_buffers(const char* filename) {
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
vector<string> read_lines(const char* filename) {
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
long long size(const char* filename) {
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

/**
 * Computes an MD5 of a given file using the MD5 library in libsrc/md5
 */
string md5(const char* filename) {
    ifstream in(filename, ios::binary);
    char buffer[64*1024];
    MD5 hasher;
    long total = 0;
    while (in) {
        in.read(buffer, 64*1024);
        hasher.update(buffer, in.gcount());    
        total+=in.gcount();
    }
    if (in.bad()) cerr << "Error reading " << filename << endl;
    hasher.finalize();
    in.close();
    return hasher.hexdigest();
}

/**
 * Computes the PFFF hash of a given file using the PFFF library.
 */
string pfff(const char* filename, int32_t key, long block_count, long block_size, long request_cost, int output_format) {
    PfffOptions opts;
    pfff_options_init(&opts, key);
    opts.output_format = output_format;
    opts.block_count = block_count;
    opts.block_size = block_size;
    opts.no_prefix = true;
    opts.no_filename = true;
    
    char result[500];
    int errorcode = pfffclib_hash_file(&opts, filename, request_cost, result, 500, NULL, 0);
    if (errorcode) {
        cerr << "Error PFFF-ing file " << filename << endl;
    }
    return string(result);
}

/**
 * Given a file, reads block_count single bytes from it.
 */
string dummy_scan(const char* filename, long block_count) {
    char bytes[block_count];
    long long fsize = size(filename);
    long pos = 0;
    long step = fsize/block_count;
    ifstream input_file(filename, ios::binary);
    for (int i = 0; i < block_count; i++) {
        input_file.seekg(pos, ios::beg);
        if(!input_file.read(&bytes[i], 1)) {
            cerr << "Error reading " << filename << endl;
            break;
        }
        pos += step;
    }
    input_file.close();
    bytes[10] = 0;
    return string(bytes);
}

}

