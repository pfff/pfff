PFFF - Probabilistic Fast File Fingerprinting
---------------------------------------------
Version 1 alpha.

COMPILATION:
	Under Linux:
		make && make tests
	
	Under Windows/MinGW
		make -f Makefile.win tests
		make -f Makefile.win

	The compiled executable (pfff) is produced in the build/ subdirectory.
	
	Compilation has so far only been tested on Win32 and Linux x86_64,
	hence bugs are possible.

USAGE:
	To obtain a basic hash of a file <filename>, pick a random number <num> and run
	
	> ./pfff -k <num>   <filename>
	
	You can provide multiple files in a single invocation.
	Numerous options for controlling the algorithm operation and output format are available.
	A list of these can be obtained by invoking
	
	> ./pfff --help

EXPERIMENTAL FEATURES:
	Note that the hashing-over-FTP feature is purely experimental: it is not guaranteed to work
	properly nor even produce correct hash (I've observed some strange cases).

COPYRIGHT AND LICENSE:
	The software copyrights belong to Konstantin Tretyakov, Pjotr Prins & Swen Laur.
	The terms of use of the software are governed by the BSD license.
	Software incorporates the following openly-licensed libraries and code:
	 *	Implementation of MD5 by Frank Thilo (http://www.bzflag.org)
	 *	Implementation of the Mersenne twister algorithm by Shawn Cokus (http://dirk.eddelbuettel.com/code/octave-mt/cokus.c.txt)
	 *	Poly1305 AES library by D.J.Bernstein (http://cr.yp.to/mac.html)
	 *	C++ Socket class by Rene' Nyffenegger (http://www.adp-gmbh.ch/win/misc/sockets.html)
	 *	UnitTest++ library by N.Llopis & C.Nicholson (http://unittest-cpp.sourceforge.net/)
