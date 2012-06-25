/**
 * FtpSocket.cpp: Socket for a somewhat more convenient use of the FTP protocol
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include "FtpSocket.h"
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>

std::string FtpClientSocket::GetResponse() {
    while(true) { // TODO: Limit number of iterations ?
        std::string r = ReceiveLine();
        // TODO: This is not how input validation should be done.
        if (r.size() < 5) {
            // Should never be the case. At least 5 chars (NNN<SPACE><CR>) are expected
            throw "UNSUPPORTED_RESPONSE";
        }
        if (r[3] == ' ') { // This is end-of-line response
            lastResponseLine = r;
            r.resize(3);
            return r;
        }
        else if (r[3] == '-') { // This is a continuation-response
            // Just skip it and read on
        }
        else { // Invalid
            throw "UNSUPPORTED_RESPONSE";
        }
    }
}

std::string FtpClientSocket::AnonymousLogin() {
    std::string response = GetResponse();
    if (response[0] != '2') throw "UNSUPPORTED_RESPONSE";
    response = SendCommand("USER anonymous");
    if (response[0] == '2') { // Login successful
        return response;
    }
    else if (response[0] == '3') { // Password required
        response = SendCommand("PASS ftp@example.com");
        if (response[0] == '2') {
            return response;
        }
        else throw "ANONYMOUS_LOGIN_FAILED";
    }
    else throw "UNSUPPORTED_RESPONSE";
}

unsigned long long FtpClientSocket::Size(const char* filename) {
    std::string cmd = "SIZE ";
    std::string response = SendCommand(cmd + filename);
    while(true) { // Ignore "Failed writing network stream" responses. TODO: Limit number of iterations?
        if (response == "426" || response == "226") response = GetResponse();
        else break;
    }
    if (response[0] != '2') throw "OPERATION_FAILED";
    unsigned long long result;
    int numscanned = std::sscanf(lastResponseLine.c_str() + 4, "%llu", &result);
    if (numscanned != 1) throw "UNSUPPORTED_RESPONSE";
    return result;
}

void FtpClientSocket::Abort() {
    std::string response = SendCommand("ABOR");
}

SocketClient* FtpClientSocket::PasvRestRetrX(const char* filename, unsigned long long rest) {
    // Start with a PASV
    std::string response = SendCommand("PASV");
    while(true) { // Ignore "Failed writing network stream" responses. TODO: Limit number of iterations?
        if (response == "426" || response == "226") {
            // First wait for another response
            //int haveResponse = Peek(100);
            //if (haveResponse == 1) {
                response = GetResponse(); // Good, got response
            //}
            /*else {
                // Let's resend the command
                std::cout << "Resending" << std::endl;
                response = SendCommand("PASV");
                std::cout << "OK" << std::endl;
            }*/
        }
        else break;
    }
    if (response[0] != '2') throw "OPERATION_FAILED";

    // Parse the (ip1,ip2,ip3,ip4,p1,p2) part	
    const char* responseLine = lastResponseLine.c_str();
    const char* start = strchr(responseLine, '(');
    if (start == NULL) throw "UNSUPPORTED_RESPONSE";
    int ip1,ip2,ip3,ip4,p1,p2;
    int numread = std::sscanf(start, "(%d,%d,%d,%d,%d,%d)", &ip1,&ip2,&ip3,&ip4,&p1,&p2);
    if (numread != 6) throw "UNSUPPORTED_RESPONSE";

    // Get the host and port parts
    uint32_t host = (ip1 << 24) | (ip2 << 16) | (ip3 << 8) | ip4;
    uint16_t port = (p1 << 8) | p2;
    
    // Open connection
    SocketClient* result = new SocketClient(host, port);
  
    // Send rest command
    if (rest != 0) {
        std::ostringstream os;
        os << "REST " << rest;
        response = SendCommand(os.str());
        while(true) { // Ignore "Failed writing network stream" responses. TODO: Limit number of iterations?
            if (response == "426" || response == "226") response = GetResponse();
            else break;
        }
        if (response[0] != '3') throw "UNSUPPORTED_RESPONSE"; // Expecting 350 restart position accepted
    }
    
    // Start transfer
    std::ostringstream os;
    os << "RETR " << filename;
    response = SendCommand(os.str());
    while(true) { // Ignore "Failed writing network stream" responses. TODO: Limit number of iterations?
        if (response == "426" || response == "226") response = GetResponse();
        else break;
    }
    if (response[0] != '1') throw "UNSUPPORTED_RESPONSE"; // Expecting 150 opening BINARY mode data connection
    return result;
}

