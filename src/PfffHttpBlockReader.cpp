/**
 * Copyright: 2009-2011, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include "PfffHttpBlockReader.h"
#include "PfffHasher.h"
#include <iostream>

// ------------- HttpBlockReader --------------

HttpBlockReader::HttpBlockReader(HttpClientSocket* http_connection, const char* filename):
    BlockReader(filename),
    http_connection(http_connection) { };

HttpBlockReader::~HttpBlockReader() {
}

long long HttpBlockReader::_size() {
    try {
        return http_connection->Size(filename.c_str());
    }
    catch(const char* e) {
        error_message = e;
        return BlockReader::READ_ERROR;
    }
}

void HttpBlockReader::begin_block_sequence(char* buffer) {
    this->buffer = buffer;
    ranges.str("");
    total_block_size = 0;
}

bool HttpBlockReader::next_block(unsigned long long block_start, unsigned long block_size) {
    if ((int)ranges.tellp() != 0) ranges << ","; 
    ranges << block_start << "-" << (block_start+block_size-1);
    total_block_size += block_size;
    return true;
}

bool HttpBlockReader::end_block_sequence() {
    // Perform the request
    try {
         string result = http_connection->RequestRanges(filename.c_str(), ranges.str());
         //std::cout << "Resultsize: " << result.size() << " expected: " << total_block_size << std::endl;
         if (result.size() != total_block_size) {
            throw (const char*)"Inconsistent number of bytes read via HTTP";
         }
         else {
            memcpy(buffer, result.c_str(), result.size());
         }
    }
    catch(const char* e) {
        //std::cout << "Here!" << std::endl;
        error_message = e;
        return BlockReader::READ_ERROR;
    }
}

