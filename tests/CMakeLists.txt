cmake_minimum_required(VERSION 2.6)
project(PfffTest)

add_subdirectory(libsrc/UnitTest++)
include_directories(libsrc/UnitTest++/src)

add_subdirectory(src)
