/**
 * disk-read-experiment-3.cpp
 *
 * The program takes a list of files, and for each file simply reads <k> kilobytes from the beginning, measuring time.
 *
 */
#include <iostream>
#include <vector>
#include <cstdlib>
#include "TimeHelpers.h"
#include "utils.h"
using std::cout;
using std::cerr;
using std::endl;
using std::rand;
using std::ifstream;
using FileUtils::read_lines;
using FileUtils::size;
using FileUtils::flush_buffers;

// Sample parameters from this set
const int BS_LEN = 44;
const int BS[] = {  1024, 2*1024, 3*1024, 4*1024, 5*1024, 6*1024, 7*1024, 8*1024, 9*1024, 
                    10*1024, 20*1024, 30*1024, 40*1024, 50*1024, 60*1024, 70*1024, 80*1024, 90*1024, 
                    100*1024, 200*1024, 300*1024, 400*1024, 500*1024, 600*1024, 700*1024, 800*1024, 900*1024, 1000*1024,
                    1100*1024, 1200*1024,1300*1024,1400*1024,1500*1024,1600*1024,1700*1024,1800*1024,1900*1024,2000*1024,
                    3000*1024,4000*1024,5000*1024,6000*1024,7000*1024,8000*1024 };

#define PICK_RANDOM(x) x[rand() % x##_LEN]; 

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: disk-read-experiment <file-with-list-of-files-to-test>" << endl;
        return 0;
    }
    
    char* buffer = new char[8000*1024];
    
    // Scan all filenames from the given file
    vector<string> filenames = read_lines(argv[1]);
    Timer timer;
    
    cout << "File_size\tBlock_size\tTime\tFile_name" << endl;
    //for (int w = 0; w < 5; w++) {
    for (int i = 0; i < filenames.size(); i++) {
        const char* filename = filenames[i].c_str();
        long bs = PICK_RANDOM(BS);
        long long fsize = size(filename);
        int cnt = 0;
        while (fsize < bs) {
            bs = PICK_RANDOM(BS);
            if (++cnt > 100) break;
        }
        if (cnt > 100) continue;
        
        flush_buffers(filename);
        ifstream in(filename, ios::binary);
        timer.Start();
                if (!in.read(buffer, bs)) {
                    cerr << "Error reading file " << filename << endl;
                }
        long long time = timer.GetTimeInMs();
        in.close();
        cout << fsize << "\t" << bs  << "\t" << time << "\t" << filename << endl;    
    }
    //}
    delete[] buffer;
    return 0;
}
