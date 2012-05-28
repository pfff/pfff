// The file was obtained from http://www.adp-gmbh.ch/win/misc/sockets.html
// and minor modifications were introduced (DEBUG variable, host field).
// The original file was accompanied with the following message
/* 
   Socket.h

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

#ifndef SOCKET_H
#define SOCKET_H

#ifdef WIN32
    #include <winsock2.h>
    #define MSG_WAITALL 0
    #define EWOULDBLOCK WSAEWOULDBLOCK
#else
    #include <arpa/inet.h>
    #include <errno.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <sys/ioctl.h>
    #include <sys/socket.h>
    #include <sys/types.h>

    #define closesocket(x) close(x)
    #define ioctlsocket(a,b,c) ioctl(a,b,c)
    typedef int SOCKET;
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
#endif

#include <string>

// Analogue of strerror
// Use: ws_strerror(ws_lasterror()) on windows or ws_strerror(errno) on linux;
std::string ws_strerror(int error_code);
int ws_lasterror();

enum TypeSocket {BlockingSocket, NonBlockingSocket};

class Socket {
public:
  // THESE ARE USED FOR DEBUGGING PURPOSES
  static bool DEBUG;  // Set to TRUE to enable debug output
  std::string host;     // Used in debug output mainly. Identifies the target host 
                        // (or LOCAL for a server socket)
  int port;
  // -----------------
  
  virtual ~Socket();
  Socket(const Socket&);
  Socket& operator=(Socket&);

  std::string ReceiveLine();
  std::string ReceiveBytes();

  // Does a non-blocking Peek for given number of iterations
  // Returns -1 if no positive response obtained during this time and 1 otherwise.
  int Peek(int iterations=100); 

  // Raw blocking recv
  int RecvBlocking(char* buffer, size_t length);
  
  void   Close();

  // The parameter of SendLine is not a const reference
  // because SendLine modifes the std::string passed.
  void   SendLine (std::string);

  // The parameter of SendBytes is a const reference
  // because SendBytes does not modify the std::string passed 
  // (in contrast to SendLine).
  void   SendBytes(const std::string&);

protected:
  //friend class SocketServer;
  //friend class SocketSelect;

  Socket(SOCKET s);
  Socket();


  SOCKET s_;

  int* refCounter_;

private:
  static void Start();
  static void End();
  static int  nofSockets_;
};

class SocketClient : public Socket {
public:
  SocketClient(const std::string& host, int port);
  SocketClient(uint32_t ip, uint16_t port);
};

/*
class SocketServer : public Socket {
public:
  SocketServer(int port, int connections, TypeSocket type=BlockingSocket);

  Socket* Accept();
};

// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winsock/wsapiref_2tiq.asp
class SocketSelect {
  public:
    SocketSelect(Socket const * const s1, Socket const * const s2=NULL, TypeSocket type=BlockingSocket);

    bool Readable(Socket const * const s);

  private:
    fd_set fds_;
}; 
*/


#endif
