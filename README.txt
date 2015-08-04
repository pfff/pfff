PFFF - Probabilistic Fast File Fingerprinting
---------------------------------------------
Version 1.0.

USAGE:
	To obtain a basic hash of a file <filename>, pick a random number <num> and run
	
	> ./pfff -k number <filename>
	
    where number is used to seed the randomizer. To recreate the pfff
    values use the same number again.
	You can provide multiple files in a single invocation.
	Numerous options for controlling the algorithm operation and output format are available.
	A list of these can be obtained by invoking
	
	> ./pfff --help

EXPERIMENTAL FEATURES:
	Note that the hashing-over-HTTP and hashing-over-FTP features are purely experimental:
    those have not been tested extensively with all possible variations of HTTP and FTP servers.
    In particular, not all HTTP servers support range requests, with those, pfff will return
    an (obscure-looking) error.

CITE:

    @article{Tretyakov:2013,
      author       = {Tretyakov, K. and Laur, S. and Smant, G. and Vilo, J. and Prins, P.},
      title        = {{Fast probabilistic file fingerprinting for big data}},
      journal      = {BMC Genomics},
      year         = {2013},
      volume       = {14 Suppl 2},
      pages        = {S8},
      doi          = {10.1186/1471-2164-14-S2-S8},
      url          = {http://www.ncbi.nlm.nih.gov/pubmed/23445565},
      abstract     = {BACKGROUND: Biological data acquisition is raising new challenges, both in data analysis and handling. Not only is it proving hard to analyze the data at the rate it is generated today, but simply reading and transferring data files can be prohibitively slow due to their size. This primarily concerns logistics within and between data centers, but is also important for workstation users in the analysis phase. Common usage patterns, such as comparing and transferring files, are proving computationally expensive and are tying down shared resources. RESULTS: We present an efficient method for calculating file uniqueness for large scientific data files, that takes less computational effort than existing techniques. This method, called Probabilistic Fast File Fingerprinting (PFFF), exploits the variation present in biological data and computes file fingerprints by sampling randomly from the file instead of reading it in full. Consequently, it has a flat performance characteristic, correlated with data variation rather than file size. We demonstrate that probabilistic fingerprinting can be as reliable as existing hashing techniques, with provably negligible risk of collisions. We measure the performance of the algorithm on a number of data storage and access technologies, identifying its strengths as well as limitations. CONCLUSIONS: Probabilistic fingerprinting may significantly reduce the use of computational resources when comparing very large files. Utilisation of probabilistic fingerprinting techniques can increase the speed of common file-related workflows, both in the data center and for workbench analysis. The implementation of the algorithm is available as an open-source tool named pfff, as a command-line tool as well as a C library. The tool can be downloaded from http://biit.cs.ut.ee/pfff.}
    }

COPYRIGHT AND LICENSE:
	The software copyrights belong to Konstantin Tretyakov, Pjotr Prins & Swen Laur.
	The terms of use of the software are governed by the BSD license.
	Software incorporates the following openly-licensed libraries and code:
	 *	Implementation of MD5 by Frank Thilo (http://www.bzflag.org)
	 *	Implementation of the Mersenne twister algorithm by Shawn Cokus (http://dirk.eddelbuettel.com/code/octave-mt/cokus.c.txt)
	 *	Poly1305 AES library by D.J.Bernstein (http://cr.yp.to/mac.html)
	 *	C++ Socket class by Rene' Nyffenegger (http://www.adp-gmbh.ch/win/misc/sockets.html)
	 *	UnitTest++ library by N.Llopis & C.Nicholson (http://unittest-cpp.sourceforge.net/)
