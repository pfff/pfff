/**
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include "output_utils.h"

// Array used by output_hex
static const char* hex_chars = "0123456789ABCDEF";

/**
 * Outputs given binary data in hex bytewise.
 */
void output_hex(ostream& out, const char* buffer, long long buffer_len) {
	for (long long i = 0; i < buffer_len; i++) {
		char a = (buffer[i] >> 4) & 0x0f;
		char b = (buffer[i] & 0x0f);
		out << hex_chars[a] << hex_chars[b];
		// TODO: It could be cool to do something like out << hex << (int)buffer[i]
		// but somehow it doesn't work as required.
	}
}
