// Test that PfffOutputFormatter behaves consistently
#include "config.h"
#include "PfffOutputFormatter.h"

TEST(TestPfffOutputFormatter) {
    PfffOptions opts;
    pfff_options_init(&opts, 1);
    
    // Test constructor
    opts.block_count = 10;
    opts.block_size = 1;
    
    PfffOutputFormatter* of = new PfffOutputFormatter(&opts);
    CHECK(dynamic_cast<Poly1305AesHasher*>(of->post_hasher)->nonce != 0);
    CHECK(of->data_len == 10);
    CHECK(of->data == of->get_content_buffer());
    memcpy(of->get_content_buffer(), "XXXXXXXXXX", 10);
    of->set_file_size(20);
    CHECK_ARRAY_EQUAL("XXXXXXXXXX", of->data, 10); // Must not change anything because opts.include_size == false
    of->set_sample_size(5);
    CHECK_ARRAY_EQUAL("XXXXX\x00\x00\x00\x00\x00", of->data, 10);
    
    // Same but for block size 2
    opts.block_size = 2;
    opts.output_format = PFO_OF_CSV;
    delete of;
    of = new PfffOutputFormatter(&opts);
    CHECK(dynamic_cast<CsvHasher*>(of->post_hasher)->opts != 0);
    CHECK(of->data_len == 20);
    CHECK(of->data == of->get_content_buffer());
    memcpy(of->get_content_buffer(), "11111111112222222222", 20);
    of->set_file_size(20);
    CHECK_ARRAY_EQUAL("11111111112222222222", of->data, 20);
    of->set_sample_size(6);
    CHECK_ARRAY_EQUAL("111111111122\x00\x00\x00\x00\x00\x00\x00\x00", of->data, 20);
    
    // Now assume we have both file size and header there too. And a strange block size.
    opts.block_size = 3;
    opts.block_count = 7;
    opts.header_block_count = 3;
    opts.with_size = true;
    delete of;
    of = new PfffOutputFormatter(&opts);
    CHECK(dynamic_cast<CsvHasher*>(of->post_hasher)->opts != 0);
    CHECK(of->data_len == 8+3*3+3*7);
    CHECK(of->data != of->get_content_buffer());
    memcpy(of->data, "11111111112222222222333333333344444444", 38);
    of->set_file_size(0x00000000);
    memcpy(of->get_content_buffer(), "HEADERHEACONTENTCONTENTC", 24);
    of->set_file_size(0x0A0B0C0D);
    of->set_sample_size(5);
    CHECK_ARRAY_EQUAL("\x0d\x0c\x0b\x0a\x00\x00\x00\x00HEADERHEACONTENTCONTENTC\x00\x00\x00\x00\x00\x00", 
                      of->data, 38);
}
