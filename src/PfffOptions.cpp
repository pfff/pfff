/**
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include "PfffOptions.h"
#include <cstring>
using std::memset;

void pfff_options_init(PfffOptions* options, uint32_t key) {
     memset(options, 0, sizeof(PfffOptions));
     options->version       = PFO_VERSION;
     options->key           = key;
     options->output_format = PFO_OF_DEFAULT;
     options->block_count   = PFO_BC_DEFAULT;
     options->block_size    = PFO_BS_DEFAULT;
     options->header_block_count = PFO_HBC_DEFAULT;
}

/**
 * Returns zero if the provided options structure is not valid (e.g. values are out of bounds, etc).
 * If the error_message parameter is not NULL, a descriptive error message is assigned to it.
 */
int pfff_options_validate(PfffOptions* options, char** error_message) {
    try {
        if (options->version != PFO_VERSION) 
            throw "Error: Invalid structure version number.";
        if (options->output_format != PFO_OF_POLY1305AES && 
            options->output_format != PFO_OF_MD5 &&
            options->output_format != PFO_OF_CSV &&
            options->output_format != PFO_OF_DEBUG)
            throw "Error: Unrecognized output format.";
        if (options->key < PFO_KEY_MIN || options->key > PFO_KEY_MAX)
            throw "Error: Invalid value for the key.";
        if (options->block_count < PFO_BC_MIN || options->block_count > PFO_BC_MAX)
            throw "Error: Invalid value for the block count parameter.";
        if (options->block_size < PFO_BS_MIN || options->block_size > PFO_BS_MAX)
            throw "Error: Invalid value for the block size parameter.";
        if (options->header_block_count < PFO_HBC_MIN || options->header_block_count > PFO_HBC_MAX)
            throw "Error: Invalid value for the header block count parameter.";
        return 1;
    }
    catch (char* msg) {
        if (error_message != NULL) (*error_message) = msg;
        return 0;
    }
}
