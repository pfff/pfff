/**
 * PfffOutputFormatter.h: Pfff hash output functionality.
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#ifndef __PfffOutputFormatter_h__
#define __PfffOutputFormatter_h__
#include "output_utils.h"
#include "PfffOptions.h"
#include "PfffPostHashing.h"
#include <string.h>

using std::string;

/**
 * Represents the algorithm options as a short string, usable for recomputing
 * or validating hashes later.
 * TODO:NB: This option-serialization is endian-specific.
 */
struct PfffOptionsSignatureStruct {
    char versionAndOutput;
    uint32_t key;
    uint16_t block_count;
    uint16_t block_size;
    uint32_t header_block_count;
    char flags;
} __attribute__((packed));

/**
 * For convenience of access to PfffOptionsSignatureStruct
 */
union PfffOptionsSignature {
    PfffOptionsSignatureStruct values;
    char text[14];
    
    inline PfffOptionsSignature() {};
    inline PfffOptionsSignature(const PfffOptions* opts) {
    	// <options> = [version:output:1][key:4][blockcount:2][blocksize:2][with_header:4][flags:1]
    	// flags = 000000[without_replacement:1bit][with_size:1bit]
    	values.versionAndOutput = (char)opts->output_format + (opts->version << 4);
    	values.key = opts->key;
    	values.block_count = opts->block_count;
    	values.block_size =  opts->block_size;
    	values.header_block_count = opts->header_block_count;
    	values.flags = (opts->without_replacement << 1) + opts->with_size;
    }
    
    /**
     * Output signature as string
     */
    inline void print(ostream& out) const {
        output_hex(out, text, sizeof(PfffOptionsSignature));
    }
} __attribute__((packed));


/**
 * PfffOutputFormatter - the class that allocates space for data, and, after this space
 * was filled with data, outputs the hash in proper format (e.g. MD5 or poly1305aes).
 * 
 * Currently it is made to account for postprocessors like the implementation of poly1305aes 
 * we're using here, which require full data to be available to perform hashing.
 * TODO: Hashing output incrementally may reduce memory requirements?
 */
class PfffOutputFormatter {
public:
    const PfffOptions* opts;
    
    // Metainfo
    PfffOptionsSignature signature;
    string filename;
    long sample_size;
    
    // Data to be hashed
    char* data;
    long data_len;
    long content_offset; // If we use file size in the hash, content_offset is 8
    // (i.e. data contains file_size first, and then follows the header & blocksample.
    // Otherwise content_offset is 0.
    
    // The post-hashing algorithm
    PostHasher* post_hasher;

    PfffOutputFormatter(const PfffOptions* opts);
    
    ~PfffOutputFormatter();
    
    /**
     * Fill header data followed by sampled data into this array.
     */
    inline char* get_content_buffer() {	return data + content_offset; }
    
    /**
     * If we wish to use file size in hashing, report it to the formatter using this value.
     */
    inline void set_file_size(long long file_size) {
        if (opts->with_size) {
            uint64_t file_size_64 = (uint64_t)file_size;
            memcpy(data, (void*)&file_size_64, 8);
        }
    }
    
    /**
     * We also need to know the file name, if the user requested to output these.
     */
    inline void set_filename(const string& filename) {
        this->filename = filename;
    }
    
    /**
     * If the size of the sample was less than block_count, tell the formatter to
     * fill the remainder of data with zeroes.
     * True sample size may not be greater than opts.block_count.
     */
    inline void set_sample_size(long sample_size) {
        long unfilled = opts->block_count - sample_size;
        this->sample_size = sample_size;
        if (unfilled > 0) {
            // Fill [content + sample_size .. content + block_count)
            memset(data + content_offset + opts->header_block_count * opts->block_size + sample_size * opts->block_size, 0, opts->block_size*unfilled);
        }
    }
    
    /**
     * When using the debug output format, we need to specify the sample itself.
     * The filename and sample_size parameters repeat those of set_file_name and set_sample_size
     * Yet otherwise you'd have to call set_* methods in a particular order (which is not cool).
     */
    inline void set_debug_info(const string& filename, const unsigned long long* sample, long sample_size) {
        if (opts->output_format == PFO_OF_DEBUG) {
            // Update debug hasher's internal state.
            (static_cast<DebugHasher*>(post_hasher))->set_debug_info(filename, sample_size, sample);
        }
    }
    
    /** 
     * Writes out the properly formatted hash.
     * The filename is needed if !opts.no_filename is given.
     * The sample information is only needed if output = debug. Otherwise
     * it can be null.
     */
    void output_hash(ostream& out) const;
};
    
#endif
