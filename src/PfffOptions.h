/**
 * The structure, holding the options for the Pfff algorithm.
 * In addition, some basic procedures are provided for initializing default values
 * and validating the structure. The functions here are all extern "C" to make it possible to
 * use the structure from within C-style linked code.
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#ifndef __PfffOptions_h__
#define __PfffOptions_h__
#include <stdint.h>


#define PFO_VERSION 1

#define PFO_OF_POLY1305AES 1
#define PFO_OF_MD5  2
#define PFO_OF_CSV  3
#define PFO_OF_DEBUG    10

#define PFO_OF_DEFAULT PFO_OF_POLY1305AES
#define PFO_BC_DEFAULT 32
#define PFO_BC_DEFAULT_SMALL 11
#define PFO_BS_DEFAULT 1
#define PFO_HBC_DEFAULT 0

#define PFO_KEY_MIN 1
#define PFO_KEY_MAX 2147483647U
#define PFO_BC_MIN  1
#define PFO_BC_MAX  65535
#define PFO_BS_MIN  1
#define PFO_BS_MAX  1023
#define PFO_HBC_MIN 0
#define PFO_HBC_MAX 1048575

struct PfffOptions {
    // Structure version, must be equal to PFO_VERSION
    unsigned char version;
    
    // Hash algorithm options
    unsigned char output_format;  // POLY1305AES=1, MD5=2, CSV=3, DEBUG=10
    uint32_t key;                 // See help.
    uint16_t block_count;
    uint16_t block_size;	
    uint32_t header_block_count;
    unsigned char without_replacement;
    unsigned char with_size;
    
    // Output options
 	unsigned char no_prefix;
    unsigned char no_filename;
} __attribute__((packed));

extern "C" {

/**
 * Initializes the options structure, setting default values. Key is the only non-default value and must therefore be specified.
 */
void pfff_options_init(PfffOptions* options, uint32_t key);

/**
 * Returns zero if the provided options structure is not valid (e.g. values are out of bounds, etc).
 * If the error_message parameter is not NULL, a descriptive error message is assigned to it.
 */
int pfff_options_validate(PfffOptions* options, char** error_message);

}

#endif
