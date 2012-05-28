// The file was obtained from http://www.adp-gmbh.ch/win/misc/sockets.html
// and minor modifications were introduced (DEBUG variable, host field).
// The original file was accompanied with the following message
/* 
   Socket.cpp

   Copyright (C) 2002-2004 Rene' Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   Rene' Nyffenegger rene.nyffenegger@adp-gmbh.ch
*/

#include "Socket.h"
#include <iostream>
#include <string.h>

using namespace std;

int Socket::nofSockets_= 0;
bool Socket::DEBUG = false;

void Socket::Start() {
  if (!nofSockets_) {
    #ifdef WIN32
    WSADATA info;
    if (WSAStartup(MAKEWORD(2,0), &info)) {
      throw "Could not start WSA";
    }
     #endif
  }
  ++nofSockets_;
}

void Socket::End() {
  #ifdef WIN32
  WSACleanup();
  #endif
}

Socket::Socket() : s_(0) {
  Start();
  // UDP: use SOCK_DGRAM instead of SOCK_STREAM
  s_ = socket(AF_INET,SOCK_STREAM,0);
  if (s_ == INVALID_SOCKET) {
    throw "INVALID_SOCKET";
  }
  refCounter_ = new int(1);
}

Socket::Socket(SOCKET s) : s_(s) {
  Start();
  refCounter_ = new int(1);
};

Socket::~Socket() {
  if (! --(*refCounter_)) {
    Close();
    delete refCounter_;
  }

  --nofSockets_;
  if (!nofSockets_) End();
}

Socket::Socket(const Socket& o) {
  refCounter_=o.refCounter_;
  (*refCounter_)++;
  s_         =o.s_;

  nofSockets_++;
}

Socket& Socket::operator=(Socket& o) {
  (*o.refCounter_)++;

  refCounter_=o.refCounter_;
  s_         =o.s_;

  nofSockets_++;

  return *this;
}

void Socket::Close() {
  	closesocket(s_);
}

std::string Socket::ReceiveBytes() {
  std::string ret;
  char buf[1024];
 
  while (1) {
    u_long arg = 0;
    if (ioctlsocket(s_, FIONREAD, &arg) != 0)
      break;

    if (arg == 0)
      break;

    if (arg > 1024) arg = 1024;

    int rv = recv (s_, buf, arg, MSG_WAITALL);
    if (rv <= 0) break;

    std::string t;

    t.assign (buf, rv);
    ret += t;
  }

  return ret;
}

int Socket::RecvBlocking(char* buffer, size_t length) {
    u_long arg = 0;
    if (ioctlsocket(s_, FIONREAD, &arg) != 0)
      std::cerr << "IOctlsocket failed" << std::endl;

    size_t recv_remaining = length;
    do {
        size_t recv_this = recv(s_, buffer, recv_remaining, MSG_WAITALL);
        if (recv_this <= 0) return recv_this;
        if (DEBUG) std::cerr << host << ":" << port << " -> " << "DATA[" << recv_this << " bytes]" << std::endl;
        recv_remaining -= recv_this;
        buffer += recv_this;
    } while (recv_remaining > 0);
    return length;
}

int Socket::Peek(int iterations) {
    char r;
    int result, i;
    for (i = 0; i < iterations; i++) {
        result = recv(s_, &r, 1, MSG_PEEK | MSG_DONTWAIT);
        if (result == 1) break;
    }
    std::cout << "Peek returning " << result << " after " << i << " iters " << std::endl;
    return result;
}

std::string Socket::ReceiveLine() {
  std::string ret;
  while (1) {
    char r;
    int result = recv(s_, &r, 1, MSG_WAITALL);
    switch(result) {
      case 0: // not connected anymore;
              // ... but last line sent
              // might not end in \n,
              // so return ret anyway.
  		if (DEBUG) std::cerr << host << ":" << port << " -> " << ret << endl;
        return ret;
      case -1:
        return "";
//      if (errno == EAGAIN) {
//        return ret;
//      } else {
//      // not connected anymore
//      return "";
//      }
    }

    ret += r;
    if (r == '\n') {
  		if (DEBUG) std::cerr << host << ":" << port << " -> " << ret;
        return ret;
    }
  }
}

void Socket::SendLine(std::string s) {
  if (DEBUG) std::cerr << host << ":" << port << " <- " << s << endl;
  s += '\n';
  send(s_,s.c_str(),s.length(),0);
}

void Socket::SendBytes(const std::string& s) {
  send(s_,s.c_str(),s.length(),0);
}

/*
SocketServer::SocketServer(int port, int connections, TypeSocket type) {
  host = "LOCAL";
  port = 0;
  
  sockaddr_in sa;

  memset(&sa, 0, sizeof(sa));

  sa.sin_family = PF_INET;             
  sa.sin_port = htons(port);          
  s_ = socket(AF_INET, SOCK_STREAM, 0);
  if (s_ == INVALID_SOCKET) {
    throw "INVALID_SOCKET";
  }

  if(type==NonBlockingSocket) {
    u_long arg = 1;
    ioctlsocket(s_, FIONBIO, &arg);
  }

  // bind the socket to the internet address
  if (bind(s_, (sockaddr *)&sa, sizeof(sockaddr_in)) == SOCKET_ERROR) {
    closesocket(s_);
    throw "INVALID_SOCKET";
  }
  
  listen(s_, connections);                               
}

Socket* SocketServer::Accept() {
  SOCKET new_sock = accept(s_, 0, 0);
  if (new_sock == INVALID_SOCKET) {
    int rc = ws_lasterror();
    if(rc == EWOULDBLOCK) {
      return 0; // non-blocking call, no request pending
    }
    else {
      throw "INVALID_SOCKET";
    }
  }

  Socket* r = new Socket(new_sock);
  return r;
}
*/

SocketClient::SocketClient(const std::string& host, int port) : Socket() {
  this->host = host;
  this->port = port;
  if (DEBUG) std::cerr << host << ":" << port << " ~~ Connect ";
  std::string error;

  hostent *he;
  if ((he = gethostbyname(host.c_str())) == 0) {
    error = strerror(errno);
    if (DEBUG) std::cerr << "FAIL (gethostbyname: " << error << ")" << std::endl;
    throw error;
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr = *((in_addr *)he->h_addr);
  memset(&(addr.sin_zero), 0, 8); 

  if (::connect(s_, (sockaddr *) &addr, sizeof(sockaddr))) {
    error = ws_strerror(ws_lasterror());
    if (DEBUG) std::cerr << "FAIL (connect: " << error << ")" << std::endl;
    throw error;
  }
  if (DEBUG) std::cerr << "OK" << std::endl;
}

SocketClient::SocketClient(uint32_t ip, uint16_t port) : Socket() {
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(ip);
  memset(&(addr.sin_zero), 0, 8); 

  this->host = inet_ntoa(addr.sin_addr);
  this->port = port;

  if (DEBUG) std::cerr << host << ":" << port << " ~~ Connect ";

  if (::connect(s_, (sockaddr *) &addr, sizeof(sockaddr))) {
    std::string error = ws_strerror(ws_lasterror());
    if (DEBUG) std::cerr << "FAIL (connect: " << error << ")" << std::endl;
    throw error;
  }
  if (DEBUG) std::cerr << "OK" << std::endl;
}


/*
SocketSelect::SocketSelect(Socket const * const s1, Socket const * const s2, TypeSocket type) {
  FD_ZERO(&fds_);
  FD_SET(const_cast<Socket*>(s1)->s_,&fds_);
  if(s2) {
    FD_SET(const_cast<Socket*>(s2)->s_,&fds_);
  }     

  TIMEVAL tval;
  tval.tv_sec  = 0;
  tval.tv_usec = 1;

  TIMEVAL *ptval;
  if(type==NonBlockingSocket) {
    ptval = &tval;
  }
  else { 
    ptval = 0;
  }

  if (select (0, &fds_, (fd_set*) 0, (fd_set*) 0, ptval) == SOCKET_ERROR) 
    throw "Error in select";
}

bool SocketSelect::Readable(Socket const* const s) {
  if (FD_ISSET(s->s_,&fds_)) return true;
  return false;
}
*/

int ws_lasterror() {
#ifdef WIN32
    return WSAGetLastError();
#else
   return errno;
#endif
}

// Taken from http://bobobobo.wordpress.com/2009/02/02/getting-winsock-error-messages-in-string-format/
std::string ws_strerror(int error_code) {
   int errCode = error_code;
   #ifdef WIN32
    // ..and the human readable error string!!
    // Interesting:  Also retrievable by net helpmsg 10060
    LPSTR errString = NULL;  // will be allocated and filled by FormatMessage

    int size = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                 FORMAT_MESSAGE_FROM_SYSTEM, // use windows internal message table
                 0,       // 0 since source is internal message table
                 errCode, // this is the error code returned by WSAGetLastError()
                          // Could just as well have been an error code from generic
                          // Windows errors from GetLastError()
                 0,       // auto-determine language to use
                 (LPSTR)&errString, // this is WHERE we want FormatMessage
                                    // to plunk the error string.  Note the
                                    // peculiar pass format:  Even though
                                    // errString is already a pointer, we
                                    // pass &errString (which is really type LPSTR* now)
                                    // and then CAST IT to (LPSTR).  This is a really weird
                                    // trip up.. but its how they do it on msdn:
                                    // http://msdn.microsoft.com/en-us/library/ms679351(VS.85).aspx
                 0,                 // min size for buffer
                 0 );               // 0, since getting message from system tables
     std::string result = string(errString);
     
     LocalFree( errString ) ; // if you don't do this, you will get an
     // ever so slight memory leak, since we asked
     // FormatMessage to FORMAT_MESSAGE_ALLOCATE_BUFFER,
     // and it does so using LocalAlloc
     // Gotcha!  I guess.
     #else
      std::string result = strerror(errCode);
     #endif
     return result;
}

