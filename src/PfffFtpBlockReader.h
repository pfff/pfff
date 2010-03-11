/**
 * PfffFtpBlockReader.h: Class for reading blocks from a file-like object on a remote server.
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#ifndef __PfffFtpBlockReader_h__
#define __PfffFtpBlockReader_h__
#include <string>
#include "FtpSocket.h"
#include "PfffBlockReader.h"

using std::string;

/**
 * A ByteAccessor for files over FTP.
 */
class FtpBlockReader: public BlockReader {
public:
	// ftp_connection must be an opened/authenticated FTP control connection socket
	// with TYPE I command executed. The block reader will not close it
	FtpBlockReader(FtpClientSocket* ftp_connection, const char* filename);
	~FtpBlockReader();
	long long _size();
	bool next_block(unsigned long long block_start, unsigned long block_size);

protected:
	FtpClientSocket* ftp_connection;
};



#endif
