PFFF - Probabilistic Fast File Fingerprinting
---------------------------------------------
Version 1.0.

USAGE:
	To obtain a basic hash of a file <filename>, pick a random number <num> and run
	
	> ./pfff <filename>
	
	You can provide multiple files in a single invocation.
	Numerous options for controlling the algorithm operation and output format are available.
	A list of these can be obtained by invoking
	
	> ./pfff --help

EXPERIMENTAL FEATURES:
	Note that the hashing-over-HTTP and hashing-over-FTP features are purely experimental:
    those have not been tested extensively with all possible variations of HTTP and FTP servers.
    In particular, not all HTTP servers support range requests, with those, pfff will return
    an error.

COPYRIGHT AND LICENSE:
	The software copyrights belong to Konstantin Tretyakov, Pjotr Prins & Swen Laur.
	The terms of use of the software are governed by the BSD license.
	Software incorporates the following openly-licensed libraries and code:
	 *	Implementation of MD5 by Frank Thilo (http://www.bzflag.org)
	 *	Implementation of the Mersenne twister algorithm by Shawn Cokus (http://dirk.eddelbuettel.com/code/octave-mt/cokus.c.txt)
	 *	Poly1305 AES library by D.J.Bernstein (http://cr.yp.to/mac.html)
	 *	C++ Socket class by Rene' Nyffenegger (http://www.adp-gmbh.ch/win/misc/sockets.html)
	 *	UnitTest++ library by N.Llopis & C.Nicholson (http://unittest-cpp.sourceforge.net/)
