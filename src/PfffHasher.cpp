/**
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include "PfffHasher.h"
#include <iostream>
using std::cout;
using std::endl;

PfffHasher::PfffHasher(const PfffOptions* opts): opts(opts) {
	formatter = new PfffOutputFormatter(opts);
	sampler = new PfffBlockSampleGenerator(opts);
}

PfffHasher::~PfffHasher() {
	delete formatter;
	delete sampler;
}

/**
 * Performs the hashing on a given input_file, writing output to a given 
 * output stream.
 * On error throws pfff_exception with the proper message.
 */
void PfffHasher::hash(ostream& out, BlockReader* input_file) {
	// Check file size
	long long size = input_file->size();
	if (size < 0) throw pfff_exception(input_file->error_message);
	else if (size == 0) {
		memset(formatter->data, 0, formatter->data_len);
	}
	else {
		// Report file features to the formatter: 
		// File size
		if(opts->with_size) 
			formatter->set_file_size(size);
		
		input_file->begin_block_sequence(formatter->get_content_buffer());
	
		// Header
		if(opts->header_block_count > 0) {
			if (!input_file->read_header(opts->block_size, opts->header_block_count)) 
				throw pfff_exception(input_file->error_message);
		}

		// Content
		// Generate block sample
		sampler->generate(size);

		if (!input_file->read_blocks(opts->block_size, sampler->sample, sampler->sample_size))
			throw pfff_exception(input_file->error_message);

		input_file->end_block_sequence();

		// If the content was too small, pad the remainder of the buffer with zeroes
		formatter->set_sample_size(sampler->sample_size);
	}
	
	if(!opts->no_filename)
		formatter->set_filename(input_file->get_filename());
	
	// If we're doing debug output, tell it to the formatter:
	if (opts->output_format == PFO_OF_DEBUG)
		formatter->set_debug_info(input_file->get_filename(), sampler->sample, sampler->sample_size);

	// Finally, output result
	formatter->output_hash(out);
}
