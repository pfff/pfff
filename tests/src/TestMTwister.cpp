// Regression-multiplatformity test for MTwister
#include "config.h"
#include "MTwister.h"
#include <limits>

const int NUM_TESTS = 5;
const int NUM_SAMPLES = 50;
uint32_t seeds[]  = { 0, 1, 1000000, std::numeric_limits<uint32_t>::max(), 1000000000 };

// Checks that several runs of MTwister result in reproducible 
// numbers (saved in $DATADIR/MTwisterRegressionTest.in)
TEST_FILEFIXTURE("TestMTwister.out", TestMTwister) {
    // Just in case (you never know where's the portability problem)
    CHECK_EQUAL(sizeof(uint32_t),4);
    CHECK_EQUAL(sizeof(uint64_t),8);

    // For each seed: create, seed, generate sequence.
    for (int i = 0; i < NUM_TESTS; i++) {
        MTwister mtwist;
        mtwist.seed(seeds[i]);
        for (int j = 0; j < NUM_SAMPLES; j++) {
            uint32_t m = mtwist.random_uint32();
            CHECK_EQUAL(next_uint32(), m);
        }
    }

    // Create once. For each seed: seed, generate sequence (a longer one this time)
    MTwister mtwist;
    for (int i = 0; i < NUM_TESTS; i++) {
        mtwist.seed(seeds[i]);
        for (int j = 0; j < NUM_SAMPLES*10; j++) {
            uint32_t m = mtwist.random_uint32();
            CHECK_EQUAL(next_uint32(), m);
        }
    }

    // Same as above, but generate uint64-s.
    for (int i = 0; i < NUM_TESTS; i++) {
        mtwist.seed(seeds[i]);
        for (int j = 0; j < NUM_SAMPLES; j++) {
            uint64_t m = mtwist.random_uint64();
            CHECK_EQUAL(next_uint64(), m);
        }
    }

    // Finally, generate uint64-s using the random_uint64_quick procedure
    for (int i = 0; i < NUM_TESTS; i++) {
        mtwist.seed(seeds[i]);
        for (int j = 0; j < NUM_SAMPLES; j++) {
            uint64_t m = mtwist.random_uint64();
            uint64_t m2 = mtwist.random_uint64_quick(m);
            CHECK_EQUAL(next_uint64(), m2);
        }
    }
}

