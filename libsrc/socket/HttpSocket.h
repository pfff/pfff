/**
 * HttpSocket.h: Socket for a somewhat more convenient use of the HTTP protocol
 * NB: This is a rather dirty solution, with numerous hacks inside.
 * Only meant to be used for debugging and experimentation purposes.
 *
 * Copyright: 2009-2012, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#ifndef __HttpSocket_h__
#define __HttpSocket_h__
#include "Socket.h"


class HttpClientSocket {
protected:
    SocketClient* _socket;
    std::string host;
    int port;
public:
    HttpClientSocket(const std::string& host, int port = 80):
        host(host), port(port), _socket(0) {};
  
    virtual ~HttpClientSocket() {
        if (_socket != 0) delete _socket;
    }

    // Performs a "range" request (PFFF-specific stuff)
    // range_string is something like "bytes=0-0,1-2,100-200"
    // Returns a std::string with all the ranges concatenated.
    std::string RequestRanges(const char* filename, std::string range_string);
    
    // Makes a request to figure out the size of the given file
    long long Size(const char* filename);
};

#endif
