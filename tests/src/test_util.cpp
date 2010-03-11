/**
 * Copyright: 2009, Konstantin Tretyakov, Pjotr Prins, Swen Laur.
 * License:   The terms of use of this software and its source code are defined by the MIT license.
 */
#include "test_util.h"

// ------------- FileFixture --------------

FileFixture::FileFixture(const char* DATA_DIR, const char* filename) {
    string data_dir(DATA_DIR);
    string file_path = data_dir + filename;
    in.open(file_path.c_str());
}
FileFixture::~FileFixture() {
    in.close();
}
uint32_t FileFixture::next_uint32() {
    uint32_t result;
    in >> result;
    return result;
}
uint64_t FileFixture::next_uint64() {
    uint64_t result;
    in >> result;
    return result;
}
string FileFixture::next_line(ifstream& infile) {
    string result;
    getline(infile, result);
    // Our files were made on windows so we might have end of line problems
    if (result[result.size()-1] == '\n' || result[result.size()-1] == '\r')
        result.resize(result.size()-1);
    return result;
}

// ------------- TwoFileFixture --------------
TwoFileFixture::TwoFileFixture(const char* DATA_DIR, const char* filename1, const char* filename2): FileFixture(DATA_DIR, filename1) {
    string data_dir(DATA_DIR);
    string file_path2 = data_dir + filename2;
    in2.open(file_path2.c_str());
}
TwoFileFixture::~TwoFileFixture() {
    in.close();
    in2.close();
}

