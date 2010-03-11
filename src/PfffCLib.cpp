/**
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include "PfffCLib.h"
#include <sstream>
#include <cstring>
#include "PfffHasher.h"
using std::ostringstream;
using std::strncpy;

/**
 * Hashes a single file. Outputs the hash into a given buffer.
 * On success returns 0. On error returns a nonzero value and sets the provided error_message variable (unless it is NULL).
 * If everything is OK except that maybe the output does not fit into the buffer, returns 0 (ERR_NONE).
 * ASSUMES the provided options are valid (i.e. call pfff_options_validate if you are not sure).
 * ASSUMES output != NULL
 */
extern "C" int pfffclib_hash_file(const PfffOptions* opts, const char* filename, unsigned long request_cost, char* output, unsigned int output_len, char* error_message, unsigned int error_message_len) {
    int result = 0;
    PfffHasher* hasher = new PfffHasher(opts);
    BlockReader* input_file = new LocalFileBlockReader(filename);
    if (request_cost > 0) input_file = new BufferingBlockReader(input_file, request_cost);
    
    ostringstream out;
    try {
        hasher->hash(out, input_file);
        strncpy(output, out.str().c_str(), output_len);
    }
    catch(pfff_exception& e) {
        if (error_message != NULL) 
            strncpy(error_message, e.what(), error_message_len);
        result = -1;
    }
    delete input_file;
    delete hasher;
    return result;
}

