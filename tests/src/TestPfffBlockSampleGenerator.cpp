// Tests basic properties of generate_sample
#include "config.h"

// Defined in PfffBlockSampleGenerator.cpp, but not normally exported outside
extern void generate_sample(uint32_t key, unsigned long n, unsigned long long min, unsigned long long max, bool with_replacement, unsigned long long* buffer);

// Tests that generate sample indeed generates samples of requested size
// with values lying within [min..max) with or without replacement
// also validate these against known test output file.
template <typename T> void test_generate_sample(T* fixture, uint32_t key, unsigned long n, unsigned long long min, unsigned long long max, bool with_replacement, bool test_replacement, bool test_limits) {
	unsigned long long buf[n+2];
	unsigned long long* buffer = buf + 1;
	buf[0] = 4242424242U;
	buf[n+1] = 4242424242U;
	generate_sample(key, n, min, max, with_replacement, buffer);
	CHECK(buf[0] == 4242424242U && buf[n+1] == 4242424242U);
	if (n > 0) CHECK(buffer[0] != 4242424242U && buffer[n-1] != 4242424242U);

	if (n > 0) {
		bool have_replacement = false;
		bool have_min = (buffer[0] == min);
		bool have_max = (buffer[0] == max-1);
		CHECK(buffer[0] >= min && buffer[0] < max);
		for (long i = 1; i < n; i++) {
			if (buffer[i] == min) have_min = true;
			if (buffer[i] == max-1) have_max = true;
			if (buffer[i] == buffer[i-1]) have_replacement = true;
			CHECK(buffer[i] >= min && buffer[i] < max);
			CHECK(buffer[i-1] <= buffer[i]);
		}
		CHECK(test_replacement == have_replacement);
		CHECK((have_min && have_max) == test_limits);
		
		// Finally, validate against known output (for portability)
		ostringstream o;
		for (long i = 0; i < n; i++) o << hex << buffer[i];
		CHECK(o.str() == fixture->next_line());
	}
}

TEST_FILEFIXTURE("TestPfffBlockSampleGenerator.out", TestPfffBlockSampleGenerator) {
	test_generate_sample(this,1, 0, 0, 0, false, false, false);
	test_generate_sample(this,1, 0, 0, 0,  true, false, false);
	test_generate_sample(this,1, 0, 1, 5, false, false, false);
	test_generate_sample(this,1, 1, 0, 1, false, false, true);
	test_generate_sample(this,1, 1, 0, 1,  true, false, true);
	test_generate_sample(this,1, 1,20,21,  true, false, true);
	test_generate_sample(this,1,100,100,200,false,false, true);
	test_generate_sample(this,1,100,100,201,false,false, true);
	test_generate_sample(this,1,100,100,205,false,false, true);
	test_generate_sample(this,1,1000,100,205,true,true, true);
	test_generate_sample(this,1,1000,0,2000000000,true,false,false);
	test_generate_sample(this,1,1000,1,5000,true,true,false);
}
