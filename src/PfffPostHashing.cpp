/**
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include "PfffPostHashing.h"

#include <ostream>
#include <stdint.h>
#include <string.h>

#include "md5.h"
#include "MTwister.h"
#include "poly1305aes_any.h"
#include "output_utils.h"

using std::ostream;

#include <iostream>
using std::cout;
using std::endl;

/**
 * On initialization creates a its 32-byte secret key using MTwister.
 * Uses zero for nonce.
 */

// ------------- Poly1305AesHasher -----------

Poly1305AesHasher::Poly1305AesHasher(uint32_t key) {
    // Nonce will be 0 for all messages
    memset(nonce, 0, 16);
    
    // Generate secret key using mtwist on a given key
    MTwister mtwist;
    mtwist.seed(key);
    for (int i = 0; i < 8; i++) {
        uint32_t l = mtwist.random_uint32();
        memcpy(secret_key + i*4, (char*)&l, 4);
    }
    poly1305aes_clamp(secret_key);
}

void Poly1305AesHasher::output_hash(ostream& out, const char* data, long data_len) const {
    unsigned char output[16];
    poly1305aes_authenticate(output, secret_key, nonce, (unsigned char*)data, data_len);
    output_hex(out, (char*)output, 16);
}


// ---------------- Md5Hasher -------------

void Md5Hasher::output_hash(ostream& out, const char* data, long data_len) const {
    MD5 md5;
    md5.update(data, data_len);
    md5.finalize();
    out << md5.hexdigest();
}


// -------------- CsvHasher --------------

void CsvHasher::output_hash(ostream& out, const char* data, long data_len) const {
    int i;
    long cur_offset = 0;

    if (opts->with_size) {
        output_hex(out, data, 8);
        out << "|";
        cur_offset += 8;
    }
    
    if (opts->header_block_count) {
        for (i = 0; i < opts->header_block_count - 1; i++) {
            output_hex(out, data + cur_offset + i*opts->block_size, opts->block_size);
            out << ",";
        }
        output_hex(out, data + cur_offset + i*opts->block_size, opts->block_size);
        out << "|";
        cur_offset += opts->header_block_count * opts->block_size;
    }
    
    for (i = 0; i < opts->block_count-1; i++) {
        output_hex(out, data + cur_offset + i*opts->block_size, opts->block_size);
        out << ",";
    }
    output_hex(out, data + cur_offset + i*opts->block_size, opts->block_size);
}


// ------------- DebugHasher ------------

/** 
 * Needs to know the options in order to format csv output properly
 */
DebugHasher::DebugHasher(const PfffOptions* opts): opts(opts) {
}

void DebugHasher::output_hash(ostream& out, const char* data, long data_len) const {
    long cur_offset = 0;
    if (!opts->no_filename) out << "FILE:\t" << filename << endl;
    if (!opts->no_prefix) {
        out << "SIGNATURE:\t";
        out << "Ver=" << opts->version << ';';
        out << "Out=" << opts->output_format << ';';
        out << "Key=" << opts->key << ';';
        out << "N="   << opts->block_count << ';';
        out << "BSz=" << opts->block_size << ';';
        out << "Hdr=" << opts->header_block_count << ';';
        out << "FSz=" << (opts->with_size ? 'y' : 'n') << ';';
        out << "Rpl=" << (opts->without_replacement ? 'n' : 'y') << ';';
        out << endl;
    }
    if (opts->with_size) {
        out << "FILE SIZE:\t";
        output_hex(out, data, 8);
        cur_offset += 8;
        out << " (" << *((long long*)(data)) << ")" << endl;
    }
    if (opts->header_block_count) {
        long header_size = opts->header_block_count*opts->block_size;
        out << "HEADER (" << header_size  << " bytes):\n";
        output_hex(out, data+cur_offset, header_size);
        cur_offset += header_size;
        out << endl;
    }
    long data_size = opts->block_count*opts->block_size;
    out << "DATA (" << data_size << " bytes):\n";
    output_hex(out, data+cur_offset, data_size);
    out << endl;
    out << "SAMPLE:\n";
    for (int i = 0; i < sample_size; i++) {
        out << sample[i] << " ";
    }
    out << endl;
}
