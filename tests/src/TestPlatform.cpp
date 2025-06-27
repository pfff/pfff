// Some platform-specificity tests
#include "config.h"

#include <sys/stat.h>
#ifdef __MINGW32__
    // Mingws uses struct _stati64 and function _stati64, in place of POSIX's stat64
    #define stat64 _stati64
#endif
#ifdef __CYGWIN__
	// struct stat64 is not used in Cygwin, just use struct stat. It's 64 bit aware.
	// http://www.cygwin.com/faq/faq.programming.html#faq.programming.stat64
	#define stat64 stat
#endif
#ifdef __APPLE__
    // stat64 is not used on Darwin, just use struct stat which expands to __DARWIN_STRUCT_STAT64
    #define stat64 stat
#endif

TEST(TestPlatform) {
    // Make sure that stat64.st_size is 64-bit
    struct stat64 stat;
    CHECK(sizeof(stat.st_size) == 8);
}
