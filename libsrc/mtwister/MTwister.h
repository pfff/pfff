// This file was obtained from "http://dirk.eddelbuettel.com/code/octave-mt/cokus.c.txt"
// and it initially came with a header message presented below.
// It was then wrapped in an OO-form by Konstantin Tretyakov (http://kt.pri.ee)

// -------------------- Original header --------------------------
// This is the ``Mersenne Twister'' random number generator MT19937, which
// generates pseudorandom integers uniformly distributed in 0..(2^32 - 1)
// starting from any odd seed in 0..(2^32 - 1).  This version is a recode
// by Shawn Cokus (Cokus@math.washington.edu) on March 8, 1998 of a version by
// Takuji Nishimura (who had suggestions from Topher Cooper and Marc Rieffel in
// July-August 1997).
//
// Effectiveness of the recoding (on Goedel2.math.washington.edu, a DEC Alpha
// running OSF/1) using GCC -O3 as a compiler: before recoding: 51.6 sec. to
// generate 300 million random numbers; after recoding: 24.0 sec. for the same
// (i.e., 46.5% of original time), so speed is now about 12.5 million random
// number generations per second on this machine.
//
// According to the URL <http://www.math.keio.ac.jp/~matumoto/emt.html>
// (and paraphrasing a bit in places), the Mersenne Twister is ``designed
// with consideration of the flaws of various existing generators,'' has
// a period of 2^19937 - 1, gives a sequence that is 623-dimensionally
// equidistributed, and ``has passed many stringent tests, including the
// die-hard test of G. Marsaglia and the load test of P. Hellekalek and
// S. Wegenkittl.''  It is efficient in memory usage (typically using 2506
// to 5012 bytes of static data, depending on data type sizes, and the code
// is quite short as well).  It generates random numbers in batches of 624
// at a time, so the caching and pipelining of modern systems is exploited.
// It is also divide- and mod-free.
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Library General Public License as published by
// the Free Software Foundation (either version 2 of the License or, at your
// option, any later version).  This library is distributed in the hope that
// it will be useful, but WITHOUT ANY WARRANTY, without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
// the GNU Library General Public License for more details.  You should have
// received a copy of the GNU Library General Public License along with this
// library; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA 02111-1307, USA.
//
// The code as Shawn received it included the following notice:
//
//   Copyright (C) 1997 Makoto Matsumoto and Takuji Nishimura.  When
//   you use this, send an e-mail to <matumoto@math.keio.ac.jp> with
//   an appropriate reference to your work.
//
// It would be nice to CC: <Cokus@math.washington.edu> when you write.
//

#ifndef __MTwister_h__
#define __MTwister_h__
#include <stdint.h>

#define hiBit(u)       ((u) & 0x80000000U)   // mask all but highest   bit of u
#define loBit(u)       ((u) & 0x00000001U)   // mask all but lowest    bit of u
#define loBits(u)      ((u) & 0x7FFFFFFFU)   // mask     the highest   bit of u
#define mixBits(u, v)  (hiBit(u)|loBits(v))  // move hi bit of u to hi bit of v

class MTwister {
public:
	static const int N = 624;              // length of state vector
	static const int M = 397;              // a period parameter
	static const uint32_t K = 0x9908B0DFU; // a magic constant

	uint32_t   state[N+1];     // state vector + 1 extra to not violate ANSI C
	uint32_t   *next;          // next random value is computed from here
	int        left;           // can *next++ this many times before reloading
	
	MTwister(): left(-1) {}
	
	void seed(uint32_t seed);
	
	uint32_t reload();
	
	inline uint32_t random_uint32() {
	    uint32_t y;
	
	    if(--left < 0) return(reload());
	
	    y  = *next++;
	    y ^= (y >> 11);
	    y ^= (y <<  7) & 0x9D2C5680U;
	    y ^= (y << 15) & 0xEFC60000U;
	    return(y ^ (y >> 18));
	}

   	inline uint64_t random_uint64() {
		uint64_t result = random_uint32();
		result <<= 32;
		result = result + random_uint32();
		return result;
	}
	
	/**
	 * Generate an approximately uniformly random ulonglong from [0..max)
	 * Note - we use (randomlonglong % range) to compute the value and hence
	 * the result is slightly nonuniform (notably nonuniform if max is large).
	 * But it's fast and should be OK for most purposes.
	 */
	inline uint64_t random_uint64_quick(uint64_t max) {
		return random_uint64() % max;
	}
};

#endif
