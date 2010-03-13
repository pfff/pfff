/**
 * pfff-timings.cpp: Timing measurements for pfff hasher
 *
 * This program takes a list of files for hashing and builds a table with the following entries:
 *   FILE_SIZE  BLOCK_COUNT  BLOCK_SIZE  REQUEST_COST  TIME
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "PfffCLib.h"
#include "TimeHelpers.h"
#include "utils.h"
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::vector;

// Vary parameters over this set:
const int BC_LEN = 14;
const int BC[] = { 1, 5, 10, 50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000 };
const int BS_LEN = 4;
const int BS[] = { 1, 10, 100, 1000 };
const int RC_LEN = 3;
const int RC[] = { 0, 8192, 65536 };
const int KEY_LEN = 1;
const int KEY[] = {1, 2};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: pfff-timings <file-with-list-of-files-to-test>" << endl;
        return 0;
    }
    
    // Scan all filenames from the given file
    vector<string> filenames = getlines(argv[1]);
    Timer timer;
    
    // Now list all parameter combinations
    long total = KEY_LEN * (BC_LEN-4) * BS_LEN * RC_LEN * filenames.size();
    long processed = 0;
    cout << "File_size\tBlock_count\tBlock_size\tRequest_cost\tTime" << endl;
    for (int bc_i = 4; bc_i < BC_LEN; bc_i++) 
        for (int bs_i = 0; bs_i < BS_LEN; bs_i++)
            for (int rc_i = 0; rc_i < RC_LEN; rc_i++)
                for (int k_i = 0; k_i < KEY_LEN; k_i++) 
                    for (int f_i = 0; f_i < filenames.size(); f_i++)
                    {
                        PfffOptions opts;
                        pfff_options_init(&opts, KEY[k_i]);
                        opts.block_count = BC[bc_i];
                        opts.block_size = BS[bs_i];
                        opts.no_prefix = true;
                        opts.no_filename = true;
                        
                        const char* file_name = filenames[f_i].c_str();
                        char result[500];
                        long long fsize = file_size(file_name);                        
                        
                        flush_file_buffers(file_name);        
                        timer.Start();
                        pfffclib_hash_file(&opts, file_name, RC[rc_i], result, 500, NULL, 0);
                        long long time = timer.GetTimeInMs();
                        cout << fsize << "\t" << BC[bc_i] << "\t" << BS[bs_i] << "\t" << RC[rc_i] << "\t" << time << "\t" << file_name << "\t" << result;                        
                        processed++;
                        cout << "\t" << processed << "/" << total << endl;
                    }
    return 0;
}
