/**
 * PfffBlockSampleGenerator.h: Random sample generator for Pfff hashing.
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#ifndef __PfffBlockSampleGenerator_h__
#define __PfffBlockSampleGenerator_h__
#include "PfffOptions.h"

/**
 * A class for generating block samples of the required size and format.
 */ 
class PfffBlockSampleGenerator {
public:
	const PfffOptions* opts;
	unsigned long long* sample;
	unsigned long sample_size; // The true sample size may be less than opts.block_count
	                           // (e.g. when sampling without replacement from a small file
					           // or when header blocks cover the whole file). 
	
	PfffBlockSampleGenerator(const PfffOptions* opts);
	
	~PfffBlockSampleGenerator();
	
	/**
	 * Given the size of the file of interest in bytes, generates the required
	 * sample of blocks (fills the sample array and sample_size variable).
	 */
	void generate(long long size_in_bytes);
};

#endif
