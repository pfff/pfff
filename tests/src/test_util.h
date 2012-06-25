/**
 * test_util.h: Utilities for use with UnitTest++.
 *
 * Copyright: 2009, Konstantin Tretyakov, Pjotr Prins, Swen Laur.
 * License:   The terms of use of this software and its source code are defined by the MIT license.
 */
#ifndef __test_util_h__
#define __test_util_h__
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include "UnitTest++.h"
#include "stdint.h"
using namespace std;


/**
 * Macros for using FileFixtures with TEST_FIXTURE.
 * Example: TEST_FILEFIXTURE("filename.out", TestName)
 * or:      TEST_TWOFILEFIXTURE("filename.in", "filename.out", TestName)
 */
#define TEST_FILEFIXTURE(Filename, Name)                    \
    struct FileFixtureFor##Name: public FileFixture {       \
        FileFixtureFor##Name() : FileFixture(DATA_DIR, Filename) {};  \
    };                                                      \
    TEST_FIXTURE(FileFixtureFor##Name, Name)

#define TEST_TWOFILEFIXTURE(Filename1, Filename2, Name)        \
    struct FileFixtureFor##Name: public TwoFileFixture {       \
        FileFixtureFor##Name() : TwoFileFixture(DATA_DIR, Filename1, Filename2) {};  \
    };                                                      \
    TEST_FIXTURE(FileFixtureFor##Name, Name)



/**
 * Fixture used for tests that wish to validate that they generate
 * a sequence of values equal to one from a file.
 */
struct FileFixture {
    ifstream in;
    
    FileFixture(const char* DATA_DIR, const char* filename);
    ~FileFixture(); // Note: we don't care about virtual methods here
    inline bool eof() { return in.eof(); }
    inline string next_line() { return next_line(in); }
    uint32_t next_uint32();
    uint64_t next_uint64();
protected:
    string next_line(ifstream& infile);
};

/** 
 * Fixture used for tests that wish to validate that require
 * two input files (one to read input and one to read output to validate against)
 */
struct TwoFileFixture: public FileFixture {
    ifstream in2;
    
    TwoFileFixture(const char* DATA_DIR, const char* filename1, const char* filename2);
    ~TwoFileFixture();
    inline bool eof_in() { return eof(); }
    inline string next_line_in() { return next_line(); }
    inline string next_line_out() { return next_line(in2); }
};

#endif
