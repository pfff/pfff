/**
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include "PfffBlockReader.h"
#include <errno.h>
#include <fstream>
#include <string.h>

using std::ifstream;
using std::ios;

// ------------- BlockReader ----------------

string BlockReader::get_filename() {
    return filename;
}

void BlockReader::begin_block_sequence(char* buffer) {
    this->buffer = buffer;
}

bool BlockReader::end_block_sequence() {
    return true;
}

bool BlockReader::read_blocks(unsigned long block_size, unsigned long long* block_indexes, unsigned long n_indexes) {
    for (long i = 0; i < n_indexes; i++) {
        bool success = next_block(block_indexes[i]*block_size, block_size);
        if (!success) return false;
    }
    return true;
}

bool BlockReader::read_header(unsigned long block_size, unsigned long n_blocks) {
    return next_block(0, n_blocks*block_size);
}

// ------------- LocalFileBlockReader -------------
LocalFileBlockReader::LocalFileBlockReader(const char* filename):
    BlockReader(filename),
    input_file(filename, ios::binary) { };

LocalFileBlockReader::~LocalFileBlockReader() {
    input_file.close();
};

// Read file size as a 64bit integer
// Originally we used boost::filesystem::file_size() here
// but it turns out that with minor tuning stat64 can work as well, and avoid the
// linking-to-boost overhead.
#include <sys/stat.h>
#ifdef __MINGW32__
    // Mingws uses struct _stati64 and function _stati64, in place of POSIX's stat64
    #define stat64 _stati64
#endif
#ifdef __CYGWIN__
	// struct stat64 is not used in Cygwin, just use struct stat. It's 64 bit aware.
	// http://www.cygwin.com/faq/faq.programming.html#faq.programming.stat64
	#define stat64 stat
#endif

long long LocalFileBlockReader::_size() {
    struct stat64 s;
    int result;
    for (int i = 0; i < 10; i++) { // If stat returns EAGAIN, we'll try this 10 times
        result = stat64(filename.c_str(), &s);
        if (result == 0 || errno != EAGAIN) break;
    }
    if (result != 0) {
        error_message = strerror(errno);
        return READ_ERROR;
    }
    if (!S_ISREG(s.st_mode)) {
        error_message = "Object ";
        error_message = error_message + filename + " is not a file.";
        return NOT_A_FILE;
    }
    else {
        return s.st_size;
    }
}

#include "output_utils.h"

bool LocalFileBlockReader::next_block(unsigned long long block_start, unsigned long block_size) {
    input_file.seekg(block_start, ios::beg);
    input_file.read(buffer, block_size);
    
    // Was there an error?
    if (input_file.bad()) {
        error_message = "File ";
        error_message = error_message + filename + " could not be read.";
        return false;
    }
    
    // Was there an eof?
    if (input_file.eof()) {
        long unread = block_size - input_file.gcount();
        if (unread > 0) {
            // If so, fill the remainder of the buffer block with zeroes
            memset(buffer + input_file.gcount(), 0, unread);
        }
        input_file.clear(); // Clear EOF bit (in case we'll be willing to read the last block again). http://en.wikipedia.org/wiki/Seekg.
    }
    	
    buffer += block_size;
    return true;
}

// ----------------- BufferingBlockReader ---------------------
BufferingBlockReader::BufferingBlockReader(BlockReader* reader, long request_cost, long max_request_size):
    BlockReader(""), reader(reader), request_cost(request_cost), max_request_size(max_request_size) {};

BufferingBlockReader::~BufferingBlockReader() {
    delete reader;
};

long long BufferingBlockReader::_size() {
    return reader->_size();
}

string BufferingBlockReader::get_filename() {
    return reader->get_filename();
}

void BufferingBlockReader::begin_block_sequence(char* buffer) {
    this->buffer = buffer;
    read_from = 0;
    read_to = 0;
    blocks = vector<Block>();
}


bool BufferingBlockReader::next_block(unsigned long long block_start, unsigned long block_size) {
    if (blocks.size() == 0) {
        // Queue first block
        read_from = block_start;
        read_to = block_start + block_size;
        blocks.push_back(Block(block_start, block_size));
        return true;
    }
    else {
        // NB: We assume that blocks come in an ordered sequence.
        if (block_start < read_from) {
            error_message = "INTERNAL BUG";
            return false;
        }
        
        // If the new block is sufficiently close to the end of the current region, append it
        bool is_near_block = (read_to + request_cost) >= block_start;
        // However, we don't want to stretch for too long
        bool request_size_ok = (block_start + block_size - read_from) <= max_request_size;
        
        if (is_near_block && request_size_ok) {
            // Append this block to the batch
            if (read_to < (block_start + block_size)) read_to = block_start + block_size;
            blocks.push_back(Block(block_start, block_size));
            return true;
        }
        else {
            // Process blocks queued so far
            if (!do_block_read()) return false;
            // Queue this block
            return next_block(block_start, block_size);
        }
    }
}

bool BufferingBlockReader::do_block_read() {
    unsigned long block_size = read_to - read_from;
    char* temp_buffer = new char[block_size];
    reader->begin_block_sequence(temp_buffer);
    bool result = reader->next_block(read_from, block_size);
    result = result && reader->end_block_sequence();
    if (!result) {
        error_message = reader->error_message;
        delete[] temp_buffer;
        return false;
    }
    // Fill the requested blocks
    for (vector<Block>::iterator b = blocks.begin(); b != blocks.end(); b++) {
        memcpy(buffer, temp_buffer + (b->start - read_from), b->len);
        buffer += b->len;
    }
    // Free the queue and temp buffer
    delete[] temp_buffer;
    read_from = 0;
    read_to = 0;
    blocks.clear();
    return true;
}

bool BufferingBlockReader::end_block_sequence() {
    if (blocks.size() > 0) return do_block_read();
    else return true;
}


