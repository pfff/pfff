/**
 * utils.h: Misc utilities
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#ifndef __utils_h__
#define __utils_h__
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <string.h>
#include "PfffCLib.h"
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::ifstream;
using std::ostream;
using std::ios;

namespace FileUtils {

/**
 * Flushes the system cache of a given file.
 * NB: It does not flush the hard drive device cache, hence repeated reads are still kinda faster than the first reads.
 * Nonetheless, it helps for fairer comparisons.
 * Beware: this function is Win32-specific. It simply does nothing on *nix systems.
 */
void flush_buffers(const char* filename);

/**
 * Reads lines from a given file.
 */
vector<string> read_lines(const char* filename);

/** 
 * Read file size
 */
long long size(const char* filename);


/**
 * Computes an MD5 of a given file using the MD5 library in libsrc/md5
 */
string md5(const char* filename);

/**
 * Computes the PFFF hash of a given file using the PFFF library.
 */
string pfff(const char* filename, int32_t key, long block_count, long block_size, long request_cost, int output_format = PFO_OF_POLY1305AES);

/**
 * Given a file, reads block_count single bytes from it.
 */
string dummy_scan(const char* filename, long block_count);

/**
 * A simple "signature" computed for a file, that may be used to quickly compare files to assess equality.
 */
struct FileSignature {
    // Signature consists of the following bytes taken from the file:
    // 1..9, 10*[1..9], 100*[1..9], 1000*[1..9], 10000*[1..9], 100000*[1..9], 1000000*[1..9]
    char bytes[7*9];
    int len;
    string file_name;
    
    FileSignature(const char* filename): file_name(filename) {
        long long fsize = size(filename);
        ifstream input_file(filename, ios::binary);
        
        memset(bytes, 0, 7*9);
        long exponent = 1;
        long k = 0;
        len = 0;
        while (exponent <= 1000000) {
            for (int i = 1; i < 10; i++) {
                long offset = i*exponent-1;
                if (offset < fsize) {
                    input_file.seekg(offset, ios::beg);
                    input_file.read(&bytes[k], 1);
                    k++;
                    len = k;
                }
                else {
                    exponent = 1000000;
                    break;
                }
            }
            exponent *= 10;
        }
        input_file.close();
    }
    
    int operator^(const FileSignature& fs) const {
        int min_len = len < fs.len ? len : fs.len;
        int max_len = len < fs.len ? fs.len : len;
        int numequal = 0;
        for (int i = 0; i < min_len; i++) {
            if (bytes[i] == fs.bytes[i]) numequal++;
        }
        
        // Return round("num_equal / max_len" * 1000)
        return 1000*numequal/max_len;
    }
    
    friend ostream& operator<<(ostream& out, FileSignature& fs) {
        for (int i = 0; i < fs.len; i++) {
            out << (int)fs.bytes[i];
            if (i != fs.len-1) out << ":";
        }
        return out;
    }
};


/**
 * Compares two files, returns num_equal_blocks/length_of_longer_file
 */
class FileComparison {
public:
    int block_size;
    long long len1;
    long long len2;
    long long min_len;
    long long max_len;
    long numequal;
    double similarity;
    const char* filename1;
    const char* filename2;
    
    FileComparison(const char* filepath1, const char* filepath2, int block_size): 
        block_size(block_size), filename1(filepath1), filename2(filepath2) {
        
        len1 = size(filepath1)/block_size;
        len2 = size(filepath2)/block_size;
        min_len = len1 < len2 ? len1 : len2;
        max_len = len1 < len2 ? len2 : len1;
        
        char buf1[block_size];
        char buf2[block_size];
        ifstream f1(filepath1, ios::binary);
        ifstream f2(filepath2, ios::binary);
        
        numequal = 0;
        
        for (long i = 0; i < min_len; i++) {
            f1.read(buf1, block_size);
            f2.read(buf2, block_size);
            bool equal = true;
            for (int j = 0; j < block_size; j++)
                if (buf1[j] != buf2[j]) {
                    equal = false;
                    break;
                }
            if (equal) numequal++;
            if (!f1 || !f2) {
                cerr << "Error comparing files " << filepath1 << " and " << filepath2 << endl;
                numequal = -max_len;
                break;
            }
        }
        
        f1.close();
        f2.close();
        
        similarity = double(numequal)/max_len;
    }
};

}

#endif
