// Smoke test for PfffHasher
#include "config.h"
#include "PfffHasher.h"
#include "PfffBlockReader.h"
#include "MTwister.h"

namespace TestPfffHasher {

const int   NUM_DATA = 8;
const int   DATA_LEN[] = {0, 1, 4, 31, 12, 13, 14, 42};
const char* DATA[] = { 
    "",
    "\x00",
    "\x00\x00\x00\x00",
    "\x00\x10\x20\x30\x40\x50\x60\x70\x80\x90\xa0\xb0\xc0\xd0\xe0\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff",
    "Hello, world",
    "\x00Hello, world",
    "\x00Hello, world\x00",
    "To be, or not to be, that is the question!"
};

const int NUM_KEY = 5;
const unsigned long KEY[] = { 1, 10, 2000000000U, 123456789, 2147483647 };

const int NUM_BLOCK_COUNT = 6;
const int BLOCK_COUNT[] = { 1, 2, 4, 1000, 1024, 65535 };

const int NUM_BLOCK_SIZE = 6;
const int BLOCK_SIZE[] = {1, 2, 4, 999, 1000, 1023 };

const int NUM_INCLUDE_HEADER = 6;
const int INCLUDE_HEADER[] = { 0, 1, 2, 10, 1024, 65535 };

const int NUM_INCLUDE_SIZE = 2;
const bool INCLUDE_SIZE[] = { true, false };

const int NUM_NO_FILENAME = 2;
const bool NO_FILENAME[] = { true, false };

const int NUM_FORMAT_CODE = 2;
const int FORMAT_CODE[] = { PFO_OF_POLY1305AES, PFO_OF_MD5 };

class MemoryBlockReader: public BlockReader {
public:	
    const char* mem;
    long len;
    
    MemoryBlockReader(const char* mem, long len): BlockReader("MEMORY"), mem(mem), len(len) {};
    
    bool next_block(unsigned long long block_start, unsigned long block_size) {
        // Beginning of each requested block MUST fall into existing space
        CHECK(block_start < len);
        if (block_start + block_size > len) {
            unsigned long true_len = len - block_start;
            memcpy(buffer, mem + block_start, true_len);
            buffer += true_len;
            memset(buffer, 0, block_size - true_len);
            buffer += (block_size - true_len);
        }
        else {
            memcpy(buffer, mem + block_start, block_size);
            buffer += block_size;
        }
        return true;
    }
    long long _size() { return len; }

};


TEST_FILEFIXTURE("TestPfffHasher.out", TestPfffHasher) {
    PfffOptions opts;
    pfff_options_init(&opts, 1);
    	
    // The total number of possible tests is too large, so we just sample randomly
    long total_tests = NUM_KEY * NUM_BLOCK_COUNT * NUM_BLOCK_SIZE * NUM_INCLUDE_HEADER;
    total_tests *= NUM_INCLUDE_SIZE * NUM_NO_FILENAME * NUM_FORMAT_CODE * NUM_DATA;
    long expected_tests = 50;
    long sample_period = total_tests/expected_tests;
    MTwister mtwist;
    mtwist.seed(1);
    
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
        PfffHasher* h = new PfffHasher(&opts);
        for (int data_i = 0; data_i < NUM_DATA; data_i++) {
            // Only do the 'sampled' tests
            if (mtwist.random_uint32() % sample_period != 0) continue;
            //opts.to_signature().print_debug(cout);
            BlockReader* br = new MemoryBlockReader(DATA[data_i], DATA_LEN[data_i]);
            ostringstream o;
            h->hash(o, br);
            CHECK_EQUAL(next_line(), o.str());
            delete br;
        }
        delete h;
    }
}

// We'll also test BlockReader::read_blocks, once we have the MemoryBlockReader here...
TEST(TestBlockReader) {
    BlockReader* ba = new MemoryBlockReader("0000011111222223333344444", 25);
    unsigned long long block_indexes[] = {0,0,1,1,2,2};
    char buffer[60];
    ba->begin_block_sequence(buffer);
    ba->read_blocks(10, block_indexes, 6);
    ba->end_block_sequence();
    CHECK_ARRAY_EQUAL("000001111100000111112222233333222223333344444\x00\x00\x00\x00\x00" "44444\x00\x00\x00\x00\x00", 
                    buffer, 60);
    delete ba;
}


// -------------------- Same as above, but with BufferingBlockReader wrapping the original readers
TEST_FILEFIXTURE("TestPfffHasher.out", TestPfffHasherWithBuffering) {
    PfffOptions opts;
    pfff_options_init(&opts, 1);
    	
    // The total number of possible tests is too large, so we just sample randomly
    long total_tests = NUM_KEY * NUM_BLOCK_COUNT * NUM_BLOCK_SIZE * NUM_INCLUDE_HEADER;
    total_tests *= NUM_INCLUDE_SIZE * NUM_NO_FILENAME * NUM_FORMAT_CODE * NUM_DATA;
    long expected_tests = 50;
    long sample_period = total_tests/expected_tests;
    MTwister mtwist;
    MTwister mtwist2;
    mtwist.seed(1);
    mtwist2.seed(1);
    
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
        PfffHasher* h = new PfffHasher(&opts);
        for (int data_i = 0; data_i < NUM_DATA; data_i++) {
            // Only do the 'sampled' tests
            if (mtwist.random_uint32() % sample_period != 0) continue;
            //opts.to_signature().print_debug(cout);
            long buffering_size = 1000 * (mtwist2.random_uint32() % 2) + mtwist2.random_uint32() % 100;
            BlockReader* br = new BufferingBlockReader(new MemoryBlockReader(DATA[data_i], DATA_LEN[data_i]), buffering_size);
            ostringstream o;
            h->hash(o, br);
            CHECK_EQUAL(next_line(), o.str());
            delete br;
        }
        delete h;
    }
}

TEST(TestBlockReaderWithBuffering) {
    BlockReader* ba = new BufferingBlockReader(new MemoryBlockReader("0000011111222223333344444", 25), 1, 10);
    unsigned long long block_indexes[] = {0,0,1,1,2,2};
    char buffer[60];
    ba->begin_block_sequence(buffer);
    CHECK(ba->read_blocks(10, block_indexes, 6));
    CHECK(ba->end_block_sequence());
    const char * expected = "000001111100000111112222233333222223333344444\x00\x00\x00\x00\x00" "44444\x00\x00\x00\x00\x00";
    CHECK_ARRAY_EQUAL(expected, buffer, 60);
    delete ba;
}

}
