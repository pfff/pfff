/**
 * disk-read-experiment.cpp
 *
 * The program takes a list of files, and for each files reads m blocks of size <k> with skips of size <k>, measuring time required for the read.
 *
 */
#include <iostream>
#include <vector>
#include <stdlib.h>
#include "TimeHelpers.h"
#include "utils.h"
using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using FileUtils::read_lines;
using FileUtils::size;
using FileUtils::flush_buffers;

// Sample parameters from this set
const int BC_LEN = 3;
const int BC[] = {10, 20, 30};
const int BS_LEN = 39;
const int BS[] = {  //1024, 2*1024, 3*1024, 4*1024, 5*1024, 6*1024, 7*1024, 8*1024, 9*1024, 
                    10*1024, 20*1024, 30*1024, 40*1024, 50*1024, 60*1024, 70*1024, 80*1024, 90*1024, 
                    100*1024, 200*1024, 300*1024, 400*1024, 500*1024, 600*1024, 700*1024, 800*1024, 900*1024, 1000*1024,
                    1100*1024, 1200*1024,1300*1024,1400*1024,1500*1024,1600*1024,1700*1024,1800*1024,1900*1024,2000*1024,
                    2100*1024,2200*1024,2300*1024,2400*1024,2500*1024,2600*1024,2700*1024,2800*1024,2900*1024,3000*1024 };
//const int BS[] = { 1024, 100*1024, 200*1024, 210*1024, 220*1024,230*1024,240*1024,250*1024,260*1024,270*1024,280*1024,290*1024,300*1024,310*1024,320*1024,330*1024,340*1024,350*1024,360*1024,370*1024,380*1024,390*1024,400*1024};
const int METHOD[] = { 0, 1};
const int METHOD_LEN = 2;
const char* METHOD_STR[] = {"seek", "scan"};

#define PICK_RANDOM(x) x[rand() % x##_LEN]; 

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: disk-read-experiment <file-with-list-of-files-to-test>" << endl;
        return 0;
    }
    
    char* buffer = new char[3000*1024];
    
    // Scan all filenames from the given file
    vector<string> filenames = read_lines(argv[1]);
    Timer timer;
    
    cout << "Method\tFile_size\tBlock_size\tTime\tBlock_count\tFile_name" << endl;
    for (int w = 0; w < 5; w++) {
    for (int i = 0; i < filenames.size(); i++) {
        const char* filename = filenames[i].c_str();
        long bs = PICK_RANDOM(BS);
        long bc = PICK_RANDOM(BC);
        long method = PICK_RANDOM(METHOD);
        long long fsize = size(filename); 
        while (fsize < 2*bc*bs) {
            bs = PICK_RANDOM(BS);
            bc = PICK_RANDOM(BC);
        }
        
        flush_buffers(filename);
        ifstream in(filename, ios::binary);
        timer.Start();
            for (int k = 0; k < bc; k++) {
                in.seekg((2*k + 1)*bs, ios::beg);
                if (method == 0) {
                    if (!in.read(buffer, 1)) {
                        cerr << "Error reading file " << filename << endl;
                    }
                }
                else if (!in.read(buffer, bs)) {
                    cerr << "Error reading file " << filename << endl;
                }
            }
        long long time = timer.GetTimeInMs();
        in.close();
        cout << METHOD_STR[method] << "\t" << fsize << "\t" << bs  << "\t" << time << "\t" << bc << "\t" << filename << endl;    
    }
    }
    delete[] buffer;
    return 0;
}
