/**
 * PfffOutputFormatter.h: Pfff hash output functionality.
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include "PfffOutputFormatter.h"
#include <iostream>
using std::cerr;
using std::endl;

//------------- PfffOutputFormatter -------------

PfffOutputFormatter::PfffOutputFormatter(const PfffOptions* opts): opts(opts) {
	// hash = [options_signature ':'] hash([<file size>:8] [<header>:?] <content>:?)
	
	// Initialize memory
	data_len = (opts->header_block_count + opts->block_count)*opts->block_size;
	if (opts->with_size) data_len += 8;
	data = new char[data_len];		
	content_offset = opts->with_size ? 8 : 0;
	// TODO: Most hashes are usable in an incrementable fashion, and hence would not need
	// to initialize the whole array. This would make code less memory-hungry
	// (however in most cases the array is small-sized and preallocating it might make
	// things faster).
	
	// Precompute signature
	if (!opts->no_prefix) signature = PfffOptionsSignature(opts);
	
	// Initialize post-hasher
	switch(opts->output_format) {
		case PFO_OF_CSV:
			post_hasher = new CsvHasher(opts);
			break;
		case PFO_OF_MD5:
			post_hasher = new Md5Hasher();
			break;
		case PFO_OF_POLY1305AES:
			post_hasher = new Poly1305AesHasher(opts->key);
			break;
		case PFO_OF_DEBUG:
			post_hasher = new DebugHasher(opts);
			break;
		default:
			// This should never happen
			cerr << "Error: Internal error (invalid output format at OutputFormatter::OutputFormatter())" << endl;
	}				
}

PfffOutputFormatter::~PfffOutputFormatter() {
	delete[] data;
	delete post_hasher;
};


/** 
 * Writes out the properly formatted hash.
 * The filename is needed if !opts.no_filename is given.
 * The sample information is only needed if output = debug. Otherwise
 * it can be null.
 */
void PfffOutputFormatter::output_hash(ostream& out) const {
	// Unless we use the DebugHasher (which outputs filename and hash signature automatically)
	// we need to check for these properties.
	if (opts->output_format != PFO_OF_DEBUG) {
	
		// Hash signature
		if (!opts->no_prefix) {
				signature.print(out);
				out << ':';
		}
	}
	
	post_hasher->output_hash(out, data, data_len);

	if (opts->output_format != PFO_OF_DEBUG)
		if (!opts->no_filename) out << "\t" << filename;
	
}

