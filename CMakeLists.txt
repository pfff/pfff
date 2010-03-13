cmake_minimum_required(VERSION 2.6)
project(Pfff)

# Compile optimized
set(CMAKE_C_FLAGS	"-O3")
set(CMAKE_CXX_FLAGS	"-O3")

# Libraries
add_subdirectory(libsrc/md5)
add_subdirectory(libsrc/mtwister)
add_subdirectory(libsrc/optionmanager)
add_subdirectory(libsrc/poly1305aes-embed)
add_subdirectory(libsrc/socket)

include_directories(libsrc/md5)
include_directories(libsrc/mtwister)
include_directories(libsrc/optionmanager)
include_directories(libsrc/poly1305aes-embed)
include_directories(libsrc/poly1305aes-20050218)
include_directories(libsrc/socket)

# Now collect the names of all files linked into the above libraries
# we'll link all those files into the final library
macro(GET_LIBRARY_SOURCES listvar library_name base_dir)
	get_property(sources TARGET ${library_name} PROPERTY SOURCES)
	foreach(file ${sources})
		# Somewhy CMake handles .S and other sources differently, passing full path for the former and just the name for the latter.
		string(REGEX MATCH "/" match_pathsep ${file})
		string(COMPARE EQUAL "${match_pathsep}" "/" has_pathsep)
		if(${has_pathsep})
			list(APPEND ${listvar} ${file})
		else()
			list(APPEND ${listvar} ../libsrc/${base_dir}/${file})
		endif()
	endforeach()
endmacro()

GET_LIBRARY_SOURCES(link_ins md5 md5)
GET_LIBRARY_SOURCES(link_ins mtwister mtwister)
GET_LIBRARY_SOURCES(link_ins optionmanager optionmanager)
GET_LIBRARY_SOURCES(link_ins poly1305aes poly1305aes-embed)
GET_LIBRARY_SOURCES(link_ins socket socket)

# The build script in the subdirectory knows to use the ${link_ins} variable to build a single library
# with all the stuff in it
add_subdirectory(src)

# Unit tests
include_directories(src)
add_subdirectory(tests)

enable_testing()
add_test(unit-tests tests/src/pffftest "${Pfff_SOURCE_DIR}/tests/data")
add_custom_target(testv 
	          make test "ARGS=-V"
		  DEPENDS tests/src/pffftest
		  COMMENT "Running tests (verbose output)")

# Install headers
install(DIRECTORY src/ DESTINATION include/
		FILES_MATCHING PATTERN "*.h"
		PATTERN ".svn" EXCLUDE)
install(FILES README.txt COPYRIGHT.txt DESTINATION .)

# Packaging
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Pfff - Probabilistic Fast File Fingerprinting")
set(CPACK_PACKAGE_VENDOR "Konstantin Tretyakov, Pjotr Prins, Swen Laur")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYRIGHT.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Pfff")
if(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  #set(CPACK_PACKAGE_ICON "${Pfff_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
  set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\kt.pri.ee")
  set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\kt.pri.ee")
  set(CPACK_NSIS_CONTACT "kt@ut.ee")
  set(CPACK_NSIS_MODIFY_PATH ON)
else()
  set(CPACK_STRIP_FILES "bin/pfff")
  set(CPACK_SOURCE_STRIP_FILES "")
endif()
set(CPACK_PACKAGE_EXECUTABLES "pfff" "Pfff")
include(CPack)


# Examples
add_subdirectory(examples)
