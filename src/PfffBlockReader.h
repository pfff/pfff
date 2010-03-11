/**
 * PfffBlockReader.h: Class for reading blocks from a file-like object.
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#ifndef __PfffBlockReader_h__
#define __PfffBlockReader_h__
#include <fstream>
#include <string>
#include <vector>

using std::ifstream;
using std::string;
using std::vector;

/**
 * Abstract interface used to perform sequential random byte access on files and file-like resources.
 * Real implementations are in FtpBlockReader, LocalFileBlockReader, etc.
 * Subclasses must override at least the next_block() and _size() methods.
 */
class BlockReader {
public:
    enum ErrorType { 
        NOT_FOUND = -1,
        NOT_A_FILE = -2,
        READ_ERROR = -3
    };
    string error_message; // Error message
    
    /**
     * buffer will be filled sequentially with the data requested in each
     * next_block.
     */
    BlockReader(const char* filename = NULL): c_size(-10), filename(filename) {};
    virtual ~BlockReader() {};
    
    /**
     * Returns the size of the file in bytes or an ErrorCode (a negative value) 
     * if something fails. Subclasses must override the _size() method.
     */
    inline long long size() {
        if (c_size == -10) c_size = _size();
        return c_size;
    } 
    
    /** 
     * This tells the block reader that a block sequence will now be read in to a 
     * given buffer. Each next block will be requested using next_block, and the sequence
     * will be finished when the call to end_block_sequence() is made.
     * buffer must be large enough to accomodate all data requested during calls to
     * next_block().
     */
    virtual void begin_block_sequence(char* buffer);
    
    /**
     * Tell BlockReader the next block that must be read.
     * block_start must be a byte offset 0..(size-1), and block_size - size of the
     * block in bytes. If block_start + block_size > size, the remainder of the block
     * is imitated to be filled with zeroes.
     * buffer must be valid and contain enough space to append all blocks.
     *
     * Block data will be output sequentially into a pre-specified buffer either
     * immediately or after the end_block_sequence() method is called.
     * Returns false on failure.
     */
    virtual bool next_block(unsigned long long block_start, unsigned long block_size) = 0;
    
    /**
     * Forces the BlockReader to perform the actual block read.
     * This is needed for HTTP block reader, which is better off reading several blocks in 
     * a single request, and also for "access optimizer wrapper" which can combine several reads
     * into one. After you call end_block_sequence, you must call begin_block_sequence again
     * if you wish to continue using the object.
     * Returns false on failure.
     */
    virtual bool end_block_sequence();
        
    /**
     * Returns the "file name" (which may be the URL) of the file being read 
     * in the form it was originally given (i.e. no normalization is done).
     */
    virtual string get_filename();
    
    // ----------------- Convenience wrappers ----------------
    /**
     * Regards the file as a sequence of <block_size>-byte sized blocks.
     * Given an ordered sequence of block indices (0 corresponding to the first block),
     * reads their data sequentially by calling next_block repeatedly.
     * If the last block overflows the size of the file, fills the remainder of the buffer
     * with zeroes.
     * Returns true on success.
     * The buffer is expected to be large enough to accomodate everything.
     */
    bool read_blocks(unsigned long block_size, unsigned long long* block_indexes, unsigned long n_indexes);
    
    /**
     * Same as read_blocks(block_size, 0...n), i.e. read first n blocks from the file.
     * Differs from next_block in that it fills whatever empty space with zeroes.
     */
    bool read_header(unsigned long block_size, unsigned long n_blocks);

protected:
    long long c_size;	// Cached result of the size() function
    string filename;	// Name or identifier of the file-like resource being read.
    char* buffer;		// Buffer to be filled with data
    
    /**
     * Actually compute the size of the object.
     */
    virtual long long _size() = 0;
    
    friend class BufferingBlockReader;
};


/**
 * A BlockReader for local files.
 */
class LocalFileBlockReader: public BlockReader {
public:
    ifstream input_file;
    
    LocalFileBlockReader(const char* filename);
    ~LocalFileBlockReader();
    long long _size();
    bool next_block(unsigned long long block_start, unsigned long block_size);
};

/**
 * Wraps a low-level block reader and buffers multiple block requests into single
 * requests for continuous blocks. The request_cost parameter tells the maximum number of "unneeded"
 * bytes to be read instead of making a request.
 * E.g. if request_cost is 1024, and blocks (1-2) and (1027-1028) are requested, the
 * block reader will join them together in a single request for 1-1028. For blocks further away
 * (1-2 and 1028-1029, for example), two requests will be done (because the "unneeded" region
 * 3-1027 is 1025 (hence > 1024) bytes long.
 * max_request_size specifies the maximum size of a single chunk to be requested at once.
 * BufferingBlockReader makes most sense when blocks are requested in an ordered manner.
 */
class BufferingBlockReader: public BlockReader {
public:
    /** NB: On destructor, BufferingBlockReader will destroy the wrapped reader too */
    BufferingBlockReader(BlockReader* reader, long request_cost = 1024, long max_request_size = 10000000);
    virtual ~BufferingBlockReader();
    
    long long _size();
    void begin_block_sequence(char* buffer);
    bool next_block(unsigned long long block_start, unsigned long block_size);
    bool end_block_sequence();
    string get_filename();
protected:
    struct Block {
        inline Block(unsigned long long start, unsigned long len): start(start), len(len) {};
        unsigned long long start;
        unsigned long len;
    };
    BlockReader* reader;
    long request_cost;
    long max_request_size;
    
    unsigned long long read_from;
    unsigned long long read_to;
    vector<Block> blocks;
    
    /**
     * Performs the reading of the queued blocks from the underlying BlockReader
     * Returns false on failure.
     */
    bool do_block_read();
};

#endif
