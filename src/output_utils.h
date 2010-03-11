/**
 * output_utils.h: Utility functions for formatting binary output
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#ifndef __output_utils_h__
#define __output_utils_h__
#include<ostream>
using std::ostream;

/**
 * Outputs given binary data in hex bytewise.
 */
extern void output_hex(ostream& out, const char* buffer, long long buffer_len);

#endif
