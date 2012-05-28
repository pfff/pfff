/**
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include "PfffFtpBlockReader.h"
#include "PfffHasher.h"
#include <fstream>
using std::ifstream;
using std::ios;

// ------------- FtpBlockReader --------------

FtpBlockReader::FtpBlockReader(FtpClientSocket* ftp_connection, const char* filename):
    BlockReader(filename),
    ftp_connection(ftp_connection) { };

FtpBlockReader::~FtpBlockReader() {
}

bool FtpBlockReader::next_block(unsigned long long block_start, unsigned long block_size) {
    try {
        long long file_size = size();
        long long chunk_size = file_size - block_start;
        if (chunk_size > block_size) chunk_size = block_size;
            	
        SocketClient* sc = ftp_connection->PasvRestRetrX(filename.c_str(), block_start);
    	int v = sc->RecvBlocking(buffer, chunk_size);
        if (v == SOCKET_ERROR) {
            error_message = ws_strerror(ws_lasterror());
            delete sc;
            return false;
        }
        else if (v != chunk_size) {
            error_message = "Data transfer failure";
            delete sc;
            return false;
        }
        ftp_connection->Abort();
        delete sc;
        
        // If the chunk size was smaller than block size, fill the remainder with zeroes
        if (chunk_size < block_size) memset(buffer + chunk_size, 0, block_size - chunk_size);
        buffer += block_size;
        return true;
    }
    catch (const char* e) {
        error_message = e;
        return false;
    }
    catch (string& s) {
        error_message = s;
        return false;
    }
}


long long FtpBlockReader::_size() {
    try {
        return ftp_connection->Size(filename.c_str());
    }
    catch(const char* e) {
        error_message = e;
        return BlockReader::READ_ERROR;
    }
}


