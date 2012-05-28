/**
 * FtpSocket.cpp: Socket for a somewhat more convenient use of the FTP protocol
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include "HttpSocket.h"
#include <string.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cstdio>
using std::sscanf;

long long HttpClientSocket::Size(const char* filename) {
    _socket = new SocketClient(host, port);
    std::string s = std::string("HEAD ") + filename + " HTTP/1.1\nHost: " + host;
    _socket->SendLine(std::string("HEAD ") + filename + " HTTP/1.1\r");
    _socket->SendLine(std::string("Host: ") + host + "\r");
    _socket->SendLine(std::string("\r"));

    // We'll do a very trivial type of processing -
    // simply read lines until we hit an empty line.
    // If any of the lines starts with a 'Content-Length:" prefix, we read out the size.
    // Otherwise return -1
    std::string prefix = "content-length:";
    s = _socket->ReceiveLine();
    long long result = -1;
    while (s != "") {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        if (s.substr(0, prefix.size()) == prefix) {
            if (1 != sscanf(s.c_str() + prefix.size() + 1, "%lld", &result))
                throw "UNSUPPORTED_RESPONSE";
            break;
        }
        s = _socket->ReceiveLine();
    }
    _socket->Close();
    delete _socket;
    _socket = 0;
    return result;
}

int read_int(Socket* sock) {
    int result = 0;
    std::string s = sock->ReceiveLine();
    if (1 != sscanf(s.c_str(), "%x", &result)) return 0;
    else return result;
}

// Reads chunked data
std::string read_chunked(Socket* sock) {
    std::ostringstream os;
    std::string s;
    int chunk_size = read_int(sock);
    while (chunk_size > 0) {
       //std::cout << "Chunk size: " << chunk_size << std::endl;
       char* buffer = new char[chunk_size];
       sock->RecvBlocking(buffer, chunk_size);
       os.write(buffer, chunk_size);
       s = sock->ReceiveLine(); // Should be '\r\n'
       chunk_size = read_int(sock);
    }
    return os.str();
}

// range_string is a byte-range string!
std::string HttpClientSocket::RequestRanges(const char* filename, std::string range_string) {
    _socket = new SocketClient(host, port);
    _socket->SendLine(std::string("GET ") + filename + " HTTP/1.1\r");
    _socket->SendLine("Host: " + host + "\r");
    _socket->SendLine("Range: bytes=" + range_string + "\r");
    _socket->SendLine("\r");

    long long contentLength = -1;
    int multipart = 0;
    int chunked = 0;
    std::string boundary;
    std::string result = "";
    std::string CONTENT_LENGTH = "content-length:";
    std::string CONTENT_TYPE   = "content-type:";
    std::string TRANSFER_ENCODING = "transfer-encoding:";

    // Scan first headers (until the empty line)
    std::string s = _socket->ReceiveLine();
    while (s != "\r\n") { // Empty line separates headers from content
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        if (s.substr(0, CONTENT_LENGTH.size()) == CONTENT_LENGTH) {
            if (1 != sscanf(s.c_str() + CONTENT_LENGTH.size() + 1, "%lld", &contentLength))
                throw "UNSUPPORTED_RESPONSE";
        }
        else if (s.substr(0, CONTENT_TYPE.size()) == CONTENT_TYPE) {
            // Look for "boundary"
            int boundaryPos = s.find("boundary=");
            if (boundaryPos != -1) {
                multipart = 1;
                boundary = s.substr(boundaryPos + 9);
                boundary.resize(boundary.size() - 2); // Trim CRLF from end
                boundary = std::string("--") + boundary;
            }
        }
        else if (s.substr(0, TRANSFER_ENCODING.size()) == TRANSFER_ENCODING) {
            if (s.find("chunked") != -1) chunked = 1;
        }
        s = _socket->ReceiveLine();
    }

    if (chunked) s = read_chunked(_socket);
    else if (contentLength > 0) {
        // Read to the end
        char* buffer = new char[contentLength];
        _socket->RecvBlocking(buffer, contentLength);
        std::string s = std::string(buffer, contentLength);
        delete[] buffer;
    }
    // In non-multipart requests we're done, otherwise we have to parse
    if (!multipart) result = s;
    else {
        // Now do the following simple trick to read out content:
            // Scan for boundary
            // Scan for '\r\n\r\n'
            // Read content until boundary
        int curPos = s.find(boundary);
        int contentStart, contentEnd;
        while (curPos != -1) {
           // Find contentStart (=CRLF CRLF)
           curPos = s.find("\r\n\r\n", curPos);
           if (curPos == -1) break;
           contentStart = curPos + 4;
           // Find contentEnd (=boundary)
           curPos = s.find(boundary, curPos);
           if (curPos == -1) break;
           contentEnd = curPos - 2;
           // Add block to r
           result.append(s.c_str() + contentStart, (contentEnd - contentStart));
        }
    }

    _socket->Close();
    delete _socket;
    _socket = 0;
    return result;
}

