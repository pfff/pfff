// Rudimentary functionality-test for output_hex
#include "config.h"
#include "output_utils.h"

string encode_hex(char* in, int len) {
	ostringstream os;
	output_hex(os, in, len);
	return os.str();
}

TEST(TestHexOutput) {
	CHECK_EQUAL("", encode_hex("", 0));
	CHECK_EQUAL("AA", encode_hex("\xAA", 1));
	CHECK_EQUAL("AAAA", encode_hex("\xAA\xAA", 2));
	CHECK_EQUAL("00", encode_hex("\x00", 1));
	CHECK_EQUAL("0000", encode_hex("\x00\x00",2));
	CHECK_EQUAL("AABBCC00", encode_hex("\xAA\xBB\xCC\x00", 4));
	CHECK_EQUAL("000102030405060708090A0B0C0D0E0F",
				encode_hex("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f", 16));
	CHECK_EQUAL("00010203040506070809",
				encode_hex("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f", 10));
	CHECK_EQUAL("00102030405060708090A0B0C0D0E0F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF",
				encode_hex("\x00\x10\x20\x30\x40\x50\x60\x70\x80\x90\xa0\xb0\xc0\xd0\xe0\xf0"
						   "\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff", 31));
}

