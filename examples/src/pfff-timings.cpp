/**
 * pfff-timings.cpp: Timing measurements for pfff hasher
 *
 * This program takes a file, listing files to be hashed, and hashes each file with either md5 or
 * pfff with randomly selected parameters. Outputs a table with the following entries:
 *   METHOD    FILE_SIZE  BLOCK_COUNT  BLOCK_SIZE  REQUEST_COST  TIME   FILENAME    HASH
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include <iostream>
#include <string>
#include "TimeHelpers.h"
#include "utils.h"
using std::cout;
using std::endl;
using std::ifstream;
using namespace FileUtils;

// Sample parameters from this set
const int BC_LEN = 14;
const int BC[] = { 1, 5, 10, 50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000 };
const int BS_LEN = 4;
const int BS[] = { 1, 10, 100, 1000 };
const int RC_LEN = 4;
const int RC[] = { 0, 8*1024, 64*1024, 512*1024 };
const int METHOD_LEN = 2;
const int METHOD[] = {0, 1};
const char* METHOD_STR[] = {"md5", "pfff-dummy"};

#define PICK_RANDOM(x) x[rand() % x##_LEN]; 

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: pfff-timings <file-with-list-of-files-to-test>" << endl;
        return 0;
    }
    
    // Scan all filenames from the given file
    vector<string> filenames = read_lines(argv[1]);
    Timer timer;
    
    cout << "Method\tFile_size\tBlock_count\tBlock_size\tRequest_cost\tTime\tFile_name\tHash" << endl;
    for (int i = 0; i < filenames.size(); i++) {
        const char* filename = filenames[i].c_str();
        long bc = PICK_RANDOM(BC);
        long bs = PICK_RANDOM(BS);
        long rc = PICK_RANDOM(RC);
        long method = PICK_RANDOM(METHOD);
        long long fsize = size(filename); 
        const char* method_str = METHOD_STR[method];
        string result;
        
        flush_buffers(filename);        
        timer.Start();
        if (method == 0) {
            result = md5(filename);
        }
        else {
            //result = pfff(filename, 1, bc, bs, rc);
            result = dummy_scan(filename, bc);
        }
        long long time = timer.GetTimeInMs();
        if (method == 0) {
            cout << method_str << "\t" << fsize << "\t" << "0" << "\t" << "0" << "\t" << "0" << "\t" << time << "\t" << filename << "\t" << result << endl;    
        }
        else {
            cout << method_str << "\t" << fsize << "\t" << bc  << "\t" << bs  << "\t" << rc  << "\t" << time << "\t" << filename << "\t" << result << endl;    
        }        
    }
    return 0;
}
