// Smoke test for PfffCLib on test files
// This is basically a copy of TestPfffHasherOnFiles
#include "config.h"
#include "PfffCLib.h"
#include "MTwister.h"

namespace TestPfffCLibOnFiles {

const int   NUM_DATA = 8;
const char* DATA[] = { 
	"TestPfffHasherOnFiles1.in",
	"TestPfffHasherOnFiles2.in",
	"TestPfffOptions.in",
	"TestMTwister.out",
	"TestPfffBlockSampleGenerator.out",
	"TestPfffHasher.out",
	"TestPfffOptions.out",
	"TestPostHashers.out"
};

const int NUM_KEY = 3;
const unsigned long KEY[] = { 0, 1, 2147483647U };

const int NUM_BLOCK_COUNT = 6;
const int BLOCK_COUNT[] = { 1, 2, 4, 1000, 1024, 65535 };

const int NUM_BLOCK_SIZE = 4;
const int BLOCK_SIZE[] = {1, 2, 4, 1023};

const int NUM_INCLUDE_HEADER = 5;
const int INCLUDE_HEADER[] = { 0, 1, 2, 10, 1024 };

const int NUM_INCLUDE_SIZE = 2;
const bool INCLUDE_SIZE[] = { true, false };

const int NUM_NO_FILENAME = 2;
const bool NO_FILENAME[] = { true, false };

const int NUM_FORMAT_CODE = 2;
const int FORMAT_CODE[] = { PFO_OF_POLY1305AES, PFO_OF_MD5 };

TEST_FILEFIXTURE("TestPfffHasherOnFiles.out", TestPfffHasherOnFiles) {
	PfffOptions opts;
	pfff_options_init(&opts, 1);
    
	// The total number of possible tests is too large, so we just sample randomly
	long total_tests = NUM_KEY * NUM_BLOCK_COUNT * NUM_BLOCK_SIZE * NUM_INCLUDE_HEADER;
	total_tests *= NUM_INCLUDE_SIZE * NUM_NO_FILENAME * NUM_FORMAT_CODE * NUM_DATA;
	long expected_tests = 100;
	long sample_period = total_tests/expected_tests;
	MTwister mtwist;
	mtwist.seed(1);
	MTwister mtwist2;
	mtwist2.seed(2);
	
	for (int key_i = 0; key_i < NUM_KEY; key_i++)
	for (int block_count_i = 0; block_count_i < NUM_BLOCK_COUNT; block_count_i++)
	for (int block_size_i = 0; block_size_i < NUM_BLOCK_SIZE; block_size_i++)
	for (int include_header_i = 0; include_header_i < NUM_INCLUDE_HEADER; include_header_i++)
	for (int include_size_i = 0; include_size_i < NUM_INCLUDE_SIZE; include_size_i++)
	for (int no_filename_i = 0; no_filename_i < NUM_NO_FILENAME; no_filename_i++)
	for (int format_code_i = 0; format_code_i < NUM_FORMAT_CODE; format_code_i++) {
		opts.key = KEY[key_i];
		opts.block_count = BLOCK_COUNT[block_count_i];
		opts.block_size  = BLOCK_SIZE[block_size_i];
		opts.header_block_count = INCLUDE_HEADER[include_header_i];
		opts.with_size = INCLUDE_SIZE[include_size_i];
		opts.no_filename = NO_FILENAME[no_filename_i];
		opts.output_format = FORMAT_CODE[format_code_i];
		for (int data_i = 0; data_i < NUM_DATA; data_i++) {
			// Only do the 'sampled' tests
			if (mtwist.random_uint32() % sample_period != 0) continue;
			ostringstream fn;
			fn << DATA_DIR << DATA[data_i];
			char output[1024];
			long request_cost = 1024*(mtwist2.random_uint32() % 10);
			int result = pfffclib_hash_file(&opts, fn.str().c_str(), request_cost, output, 1024, NULL, 0);
			CHECK_EQUAL(0, result);
			string expected = next_line();
			CHECK_EQUAL(expected, string(output));
		}
	}
}

};
