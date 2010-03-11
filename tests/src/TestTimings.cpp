// Timings for Poly1305Aes and Md5 on 100mb of data (doesn't really "test" anything, just outputs results)
#include "config.h"
#include <string.h>
#include "PfffPostHashing.h"

void timings(long len, char* title) {
	Md5Hasher md5;
	Poly1305AesHasher paes(1);
	ostringstream o1, o2;
	UnitTest::Timer t1, t2;

	char* memory = new char[len];
	memset(memory, 0, len);

	t1.Start();
	md5.output_hash(o1, memory, len);
	int time1 = t1.GetTimeInMs();

	t2.Start();
	paes.output_hash(o2, memory, len);
	int time2 = t2.GetTimeInMs();

	delete[] memory;
	
	cout << "MD5(" << title << "):        " << "\t" << time1 << endl;
	cout << "Poly1305AES(" << title << "):" << "\t" << time2 << endl;
}

TEST(TestTimings) {
	const long MB = 1024*1024;
	timings(1*MB, "1MB");
	timings(10*MB, "10MB");
	timings(100*MB, "100MB");	
}
