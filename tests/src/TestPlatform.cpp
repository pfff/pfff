// Some platform-specificity tests
#include "config.h"

#include <sys/stat.h>
#ifdef __MINGW32__
    // Mingws uses struct _stati64 and function _stati64, in place of POSIX's stat64
    #define stat64 _stati64
#endif


TEST(TestPlatform) {
    // Make sure that stat64.st_size is 64-bit
    struct stat64 stat;
    CHECK(sizeof(stat.st_size) == 8);
}
