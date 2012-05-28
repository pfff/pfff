/**
 * PfffHttpBlockReader.h: Class for reading blocks from a file-like object on a remote server.
 *
 * Copyright: 2009-2011, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#ifndef __PfffHttpBlockReader_h__
#define __PfffHttpBlockReader_h__
#include <string>
#include <sstream>
#include "HttpSocket.h"
#include "PfffBlockReader.h"

using std::string;
using std::ostringstream;

/**
 * A BlockReader for files over HTTP.
 */
class HttpBlockReader: public BlockReader {
public:
    // http_connection must be an instance of HttpSocket
    HttpBlockReader(HttpClientSocket* http_connection, const char* filename);
    ~HttpBlockReader();
    long long _size();
    bool next_block(unsigned long long block_start, unsigned long block_size);
    void begin_block_sequence(char* buffer);
    bool end_block_sequence();

protected:
    HttpClientSocket* http_connection;
    ostringstream ranges;
    long long total_block_size;
};



#endif
