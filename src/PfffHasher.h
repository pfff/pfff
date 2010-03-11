/**
 * PfffHasher.h: Main algorithm of Pfff hashing
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#ifndef __PfffHasher_h__
#define __PfffHasher_h__
#include <exception>
#include <ostream>
#include <string>
#include "PfffOptions.h"
#include "PfffBlockReader.h"
#include "PfffBlockSampleGenerator.h"
#include "PfffOutputFormatter.h"

using std::exception;
using std::ostream;
using std::string;

/**
 * Exception thrown by PfffHasher if something goes wrong.
 */
class pfff_exception: public exception {
public:
	string message;
	pfff_exception(const string message): message(message) {};
	virtual ~pfff_exception() throw() {};
	inline const char* what() const throw() {
		return message.c_str();
	}
};


/**
 * Encapsulates the high-level pfff hashing logic.
 * Usage: create an instance parameterized by options,
 *        and call the hash() function on a given file(s).
 */
class PfffHasher {
public:
	const PfffOptions* opts;
	PfffBlockSampleGenerator* sampler;
	PfffOutputFormatter* formatter;
	
	PfffHasher(const PfffOptions* opts);
	
	~PfffHasher();

	/**
	 * Performs the hashing on a given input_file, writing output to a given 
	 * output stream.
	 * On error throws pfff_exception with the proper message.
	 */
	void hash(ostream& out, BlockReader* input_file);
};

#endif
