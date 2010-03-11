/**
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include "PfffBlockSampleGenerator.h"
#include <set>
#include <stdint.h>
#include "MTwister.h"
using std::multiset;
using std::set;

#include <iostream>
using std::cout;
using std::endl;

/**
 * Given a random key, generate a sorted sample of n uniformly picked indices from [min..max).
 * If with_replacement is true, indices are taken with replacement.
 *
 * Uses the Mersenne twister algorithm & rather clean uniform number distribution.
 * TODO: Compare to a simple C rand() implementation for speed (should be way faster).
 * NB: When with_replacement = false and max < n the algorithm is undefined.
 * TODO: When with_replacement = false and max is close to n, the algorithm is slow
 */
void generate_sample(uint32_t key, unsigned long n, unsigned long long min, unsigned long long max, bool with_replacement, unsigned long long* buffer) {
    // Initialize prng state.
    MTwister mtwist;
    mtwist.seed(key);
    uint64_t range = (uint64_t)(max - min);
    
    if (with_replacement) {
        multiset<uint64_t> values;
        while (values.size() < n) {
            uint64_t val = min + mtwist.random_uint64_quick(range);
            values.insert(val);
        }
        long i = 0;
        for(set<uint64_t>::iterator v = values.begin(); v != values.end(); v++) {
            buffer[i++] = (unsigned long long)*v;
        }
    }
    else {
        set<uint64_t> values;
        while (values.size() < n) {
            uint64_t val = min + mtwist.random_uint64_quick(range);
            values.insert(val);
        }
        long i = 0;
        for(set<uint64_t>::iterator v = values.begin(); v != values.end(); v++) {
            buffer[i++] = (unsigned long long)*v;
        }
    }
};


// ---------------- PfffBlockSampleGenerator -----------------
PfffBlockSampleGenerator::PfffBlockSampleGenerator(const PfffOptions* opts): opts(opts) {
    sample = new unsigned long long[opts->block_count];
}

PfffBlockSampleGenerator::~PfffBlockSampleGenerator() {
    delete[] sample;
}

/**
 * Given the size of the file of interest in bytes, generates the required
 * sample of blocks (fills the sample array and sample_size variable).
 */
void PfffBlockSampleGenerator::generate(long long size_in_bytes) {
    // How many blocks are there total in the file?
    unsigned long long size_in_blocks = size_in_bytes/opts->block_size;
    if (size_in_blocks*opts->block_size < size_in_bytes) size_in_blocks++;
    
    // How many blocks will we be reading?
    sample_size = opts->block_count;
    
    // When sampling *without replacement* we can't request too many blocks. Trim.
    if (opts->without_replacement && (sample_size > size_in_blocks - opts->header_block_count)) {
        // The requested sample size to take is greater than the number of available blocks. 
        // That means that the required sample is the whole file (remaining besides the header).
        sample_size = size_in_blocks - opts->header_block_count;
        if (sample_size < 0) sample_size = 0;
        if (sample_size > 0) {
            for (long i = opts->header_block_count; i < size_in_blocks; i++) {
                sample[i] = i;
            }
        }
        return;
    }
    
    // Maybe there's nothing to sample at all?
    if (opts->header_block_count >= size_in_blocks) {
        sample_size = 0;
        return;
    }
    
    // Generate the sample
    generate_sample(opts->key, sample_size, opts->header_block_count, size_in_blocks, !opts->without_replacement, sample);
}

