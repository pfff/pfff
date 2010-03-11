// Test of Poly1305aes and MD5 post-hashers. 
// Rather rudimentary but should detect non-crossplatformness.
// CsvHasher and DebugHasher can live without a test, I think
#include "config.h"
#include "PfffPostHashing.h"

const int   NUM_TESTS = 8;
const char* TEST_DATA[] = { 
    "",
    "\x00",
    "\x00\x00\x00\x00",
    "\x00\x10\x20\x30\x40\x50\x60\x70\x80\x90\xa0\xb0\xc0\xd0\xe0\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff",
    "Hello, world",
    "\x00Hello, world",
    "\x00Hello, world\x00",
    "To be, or not to be, that is the question!"
};

const int   TEST_DATA_LEN[] = {0, 1, 4, 31, 12, 13, 14, 42};

template <typename T> void test_PostHasher(PostHasher* ph, T* fixture) {
    for (int i = 0; i < NUM_TESTS; i++) {
        ostringstream o;
        ph->output_hash(o, TEST_DATA[i], TEST_DATA_LEN[i]);
        CHECK_EQUAL(fixture->next_line(), o.str());
    }	
}

TEST_FILEFIXTURE("TestPostHashers.out", TestPostHashers) {
    PostHasher* ph;
    uint32_t keys[] = { 0, 1, 20, 500, 1000000, 2000000000 };
    for (int i = 0; i < 6; i++) {
        ph = new Poly1305AesHasher(keys[i]);
        test_PostHasher(ph, this);
        delete ph;
    }
    
    ph = new Md5Hasher();
    test_PostHasher(ph, this);
    delete ph;
}
