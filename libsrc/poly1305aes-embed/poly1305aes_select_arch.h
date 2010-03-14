#ifndef __poly1305aes_select_arch__
#define __poly1305aes_select_arch__

#if defined(__tune_pentiumpro__) || defined(__pentiumpro)
// PPro platform
	#define AES_S            "aes_ppro.s"
	#define AES_CONSTANTS_S  "aes_ppro_constants.s"
	#define POLY1305_S       "poly1305_ppro.s"
	#define POLY1305_CONSTANTS_S        "poly1305_ppro_constants.s"
	#define POLY1305AES_AUTHENTICATE_C  "poly1305aes_ppro_authenticate.c"
	#define POLY1305AES_CLAMP_C         "poly1305aes_ppro_clamp.c"
	#define POLY1305AES_ISEQUAL_S       "poly1305aes_ppro_isequal.s"
	#define POLY1305AES_VERIFY_C        "poly1305aes_ppro_verify.c"
	#define POLY1305AES_H               "poly1305aes_ppro.h"
	#define AES_C                       "empty.c"
	#define AES_CONSTANTS_C             "empty.c"
	#define POLY1305_C                  "empty.c"
	#define POLY1305_CONSTANTS_C        "empty.c"
	#define POLY1305AES_ISEQUAL_C       "empty.c"
//#if defined(__tune_athlon__) || defined(__athlon)
#elif defined(__tune_athlon__) || defined(__athlon)
// Athlon platform
    #define AES_S            "aes_athlon.s"
    #define AES_CONSTANTS_S  "aes_athlon_constants.s"
    #define POLY1305_S       "poly1305_athlon.s"
    #define POLY1305_CONSTANTS_S        "poly1305_athlon_constants.s"
    #define POLY1305AES_AUTHENTICATE_C  "poly1305aes_athlon_authenticate.c"
    #define POLY1305AES_CLAMP_C         "poly1305aes_athlon_clamp.c"
    #define POLY1305AES_ISEQUAL_S       "poly1305aes_athlon_isequal.s"
    #define POLY1305AES_VERIFY_C        "poly1305aes_athlon_verify.c"
    #define POLY1305AES_H               "poly1305aes_athlon.h"
    #define AES_C                       "empty.c"
    #define AES_CONSTANTS_C             "empty.c"
    #define POLY1305_C                  "empty.c"
    #define POLY1305_CONSTANTS_C        "empty.c"
    #define POLY1305AES_ISEQUAL_C       "empty.c"
#elif defined(__ppc__) || defined(__ppc64__)
// MacOS platform
    #define AES_S            "aes_macos.s"
    #define AES_CONSTANTS_S  "aes_macos_constants.s"
    #define POLY1305_S       "poly1305_macos.s"
    #define POLY1305_CONSTANTS_S        "poly1305_macos_constants.s"
    #define POLY1305AES_AUTHENTICATE_C  "poly1305aes_macos_authenticate.c"
    #define POLY1305AES_CLAMP_C         "poly1305aes_macos_clamp.c"
    #define POLY1305AES_ISEQUAL_S       "poly1305aes_macos_isequal.s"
    #define POLY1305AES_VERIFY_C        "poly1305aes_macos_verify.c"
    #define POLY1305AES_H               "poly1305aes_macos.h"
    #define AES_C                       "empty.c"
    #define AES_CONSTANTS_C             "empty.c"
    #define POLY1305_C                  "empty.c"
    #define POLY1305_CONSTANTS_C        "empty.c"
    #define POLY1305AES_ISEQUAL_C       "empty.c"
#elif (__DBL_MANT_DIG__ == 53)
// Generic (yet is said to not work on x86 linux and some other platforms with non-IEEE doubles)
    #define AES_S            "empty.c"
    #define AES_CONSTANTS_S  "empty.c"
    #define POLY1305_S       "empty.c"
    #define POLY1305_CONSTANTS_S        "empty.c"
    #define POLY1305AES_AUTHENTICATE_C  "poly1305aes_53_authenticate.c"
    #define POLY1305AES_CLAMP_C         "poly1305aes_53_clamp.c"
    #define POLY1305AES_ISEQUAL_S       "empty.c"
    #define POLY1305AES_VERIFY_C        "poly1305aes_53_verify.c"
    #define POLY1305AES_H               "poly1305aes_53.h"
    #define AES_C                       "aes_big.c"
    #define AES_CONSTANTS_C             "aes_big_constants.c"
    #define POLY1305_C                  "poly1305_53.c"
    #define POLY1305_CONSTANTS_C        "poly1305_53_constants.c"
    #define POLY1305AES_ISEQUAL_C       "poly1305aes_53_isequal.c"
#else
// Sorry
    #error "Unrecognized architecture. Cannot compile poly1305aes, sorry."
#endif


#endif
