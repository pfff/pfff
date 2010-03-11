/**
 * main.cpp: Pfff tests main file.
 *
 * Copyright: 2009, Konstantin Tretyakov, Pjotr Prins, Swen Laur.
 * License:   The terms of use of this software and its source code are defined by the MIT license.
 */
#include "config.h"
#include <iostream>
#ifdef _WIN32
	#include <direct.h>
	#define chdir _chdir
#else
	#include <unistd.h>
#endif
using std::cerr;
using std::endl;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: pffftest <data_file_directory>" << endl;
        return 1;
    }
    else {
        chdir(argv[1]);
        return UnitTest::RunAllTests();
    }
}
