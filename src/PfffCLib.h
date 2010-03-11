/**
 * The simplified "C" interface to the PFFF hasher functionality.
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#ifndef __PfffCLib_h__
#define __PfffCLib_h__
#include "PfffOptions.h"

/**
 * Hashes a single file. Outputs the hash into a given buffer.
 * On error returns a nonzero value and sets the provided error_message variable (unless it is NULL).
 * If everything is OK except that maybe the output does not fit into the buffer, returns 0.
 * ASSUMES the provided options are valid (i.e. call pfff_options_validate if you are not sure).
 * ASSUMES output != NULL
 * When request_cost > 0, multiple requests with gaps less than the provided number are combined in single reads.
 */
extern "C" int pfffclib_hash_file(const PfffOptions* opts, const char* filename, unsigned long request_cost, char* output, unsigned int output_len, char* error_message, unsigned int error_message_len);

#endif
