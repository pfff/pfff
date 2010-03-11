/**
 * PfffPostHashing.h: Wraps various output hashers (e.g. Poly1305Aes and MD5).
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#ifndef __PfffPostHashing_h__
#define __PfffPostHashing_h__
#include <ostream>
#include <string>
#include <stdint.h>
#include "PfffOptions.h"

using std::ostream;
using std::string;

/**
 * Abstract interface for any hasher.
 * TODO: Some hashers (e.g. MD5) can work incrementally. For these it would make 
 * sense to have an update() function followed by an output() call, instead of a single
 * output_hash().
 * But so far the implementation of Poly1305aes we're using is not supporting this
 * and in fact as we want to hash rather small amounts of data it might be faster to
 * hash as a whole.
 */
class PostHasher {
public:
	/**
	 * Given a new piece of data, hashes it and writes output to a given stream.
	 * Note that this must be a const method so that it would be reusable
	 * between invocations for different files.
	 */
	virtual void output_hash(ostream& out, const char* data, long data_len) const = 0;
	virtual ~PostHasher() {};
};

/**
 * On initialization creates a its 32-byte secret key using MTwister.
 * Uses zero for nonce.
 */
class Poly1305AesHasher: public PostHasher {
public:
	unsigned char secret_key[32];
	unsigned char nonce[16];
	
	Poly1305AesHasher(uint32_t key);
	
	void output_hash(ostream& out, const char* data, long data_len) const;
};


/**
 * Purely an interfacing wrapper around a nice MD5 class.
 */
class Md5Hasher: public PostHasher {
public:
	void output_hash(ostream& out, const char* data, long data_len) const;
};


/**
 * Does not really "hash" anything, simply outputs the data as properly
 * comma-separated values.
 */
class CsvHasher: public PostHasher {
public:
	const PfffOptions* opts;
	
	/** 
	 * Needs to know the options in order to format csv output properly
	 */
	inline CsvHasher(const PfffOptions* opts): opts(opts) {}
	
	void output_hash(ostream& out, const char* data, long data_len) const;
};



/**
 * Does not really "hash" anything, but outputs the data and a lot of debug
 * info around it.
 */
class DebugHasher: public PostHasher {
public:
	const PfffOptions* opts;
	
	string filename;
	unsigned long sample_size;
	const unsigned long long* sample;
	
	/** 
	 * Needs to know the options in order to format csv output properly
	 */
	DebugHasher(const PfffOptions* opts);
	
	/**
	 * Use this before output_hash to update filename and signature fields.
	 * (This is a hack to make this object fit with the otherwise natural 
	 *  PostHasher interface).
	 */
	inline void set_debug_info(const string& filename, unsigned long sample_size, const unsigned long long* sample) {
		this->filename = filename;
		this->sample_size = sample_size;
		this->sample = sample;
	}
	
	void output_hash(ostream& out, const char* data, long data_len) const;
};


#endif
