"Embedding wrapper" for Poly1305-AES
------------------------------------

These are the files for a somewhat more convenient compilation of Poly1305-AES library.

The original Makefile of Poly1305AES requires the availability of tools like .sh to determine the platform,
which is inconvenient if you compile on Windows.

Here we just rely on the GCC #defines to pick out the proper files (we do sacrifice some precision in the detection
procedure, but it's not a problem for us).

If you use Dev-Cpp then instead of using a Makefile, you can just as well simply
include all of the .S and .c files into your Dev-Cpp project and include this directory and the the 
poly1305aes-20050218 directory to your include path when compiling.

When using the library, use poly1303aes_any.h from this directory as a main header.

------------------------
(C) 2009, The PFFF project
