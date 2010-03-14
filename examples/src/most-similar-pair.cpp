/**
 * most-similar.cpp: Given a collection of files, finds the two most similar in terms of N-byte sized block-wise similarity.
 * Uses a fast approximate algorithm.
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include <algorithm>
#include <iostream>
#include "utils.h"
using std::cout;
using std::cerr;
using std::endl;
using std::sort;


struct ComparedPair {
    int i;
    int j;
    int similarity;
    ComparedPair(int i, int j, int similarity): i(i), j(j), similarity(similarity) {}
    
    bool operator<(const ComparedPair& o) const {
        return similarity > o.similarity;
    }
};


int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: most-similar-pair <file-with-list-of-files-to-test>" << endl;
        return 0;
    }
    
   /* long block_size = strtol(argv[2], NULL, 0);
    if (block_size < 1 || block_size > 1000) {
        cout << "Error: Block size must be between 1 and 1000" << endl;
        return 1;
    } */
    
    // Scan all filenames from the given file
    vector<string> filenames = getlines(argv[1]);
    
    // Compute a signature for each file
    cerr << "### Computing signatures" << endl;
    vector<FileSignature> signatures;
    for (int i = 0; i < filenames.size(); i++) {
        FileSignature fs = FileSignature(filenames[i].c_str());
        if (fs.len != 0) signatures.push_back(fs);
        cerr << ".";
    }
    cerr << endl;
    
    cerr << "### Computing approximate similarities" << endl;
    vector<ComparedPair> pairs;
    
    for (int i = 0; i < signatures.size(); i++) {
        cerr << ".";
        for (int j = 0; j < i; j++) {
            int sd = signatures[i] ^ signatures[j];
            pairs.push_back(ComparedPair(i, j, sd));
        }
    }
    cerr << endl;
    
    cerr << "### Computing exact similarities" << endl;
    
    int BLOCK_SIZE[] = {1, 2, 3, 4, 5, 10, 20, 30, 40, 50, 100, 200, 300, 400, 500, 1000 };
    int BLOCK_SIZE_LEN = 16;
    
    sort(pairs.begin(), pairs.end());
    cout << "File1\tFile2\tApproxSimilarity\tBlockSize\tLen1\tLen2\tNumEqual\tSimilarity" << endl;
    for (int i = 0; i < 1000; i++) {
        for (int bs = 0; bs < BLOCK_SIZE_LEN; bs++) {
            FileComparison fc(signatures[pairs[i].i].file_name.c_str(), signatures[pairs[i].j].file_name.c_str(), BLOCK_SIZE[bs]);
            cout << fc.filename1 << "\t" << fc.filename2 << "\t" << pairs[i].similarity << "\t" 
                 << fc.block_size << "\t" << fc.len1 << "\t" << fc.len2 << "\t" << fc.numequal << "\t" << fc.similarity << endl;
        }
    }
    return 0;
}
