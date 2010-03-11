/**
 * FtpSocket.h: Socket for a somewhat more convenient use of the FTP protocol
 * NB: This is a rather dirty solution, with numerous hacks inside.
 * Only meant to be used for debugging and experimentation purposes.
 *
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#ifndef __FtpSocket_h__
#define __FtpSocket_h__
#include "Socket.h"


class FtpClientSocket : public SocketClient {
public:
	std::string lastResponseLine; // This stores the last line of response read in GetResponse

	FtpClientSocket(const std::string& host, int port = 21): SocketClient(host, port) {};
	
	// Reads all lines of the response. Returns response code
	std::string GetResponse();
	
	// Performs the USER anonymous PASS pfff@ut login sequence. Returns the last response code
	// (which will be 230 if all OK, or some error code if none. Check lastResponseLine for more info)
	std::string AnonymousLogin();
	
	// Sends command and returns the resulting response code
	inline std::string SendCommand(std::string cmd) {
		this->SendLine(cmd + '\r');
		return this->GetResponse();
	}
	
	/*ClientSocket* RetrieveFile(std::string filename, unsigned long long rest = 0) {
	}*/
	
	// Returns the size of a given file on server
	unsigned long long Size(const char* filename);
	
	
	// Does a sequence of PASV/REST/RETR filename commands. Returns the opened socket.
	// Silently ignores all 426 (Failure writing network stream) responses that might come due to dropped
	// data connections and 226 (File transfer successful) - that's why the "X" in the title.
	// The caller is responsible for freeing the socket object.
	SocketClient* PasvRestRetrX(const char* filename, unsigned long long rest = 0);
};

#endif
