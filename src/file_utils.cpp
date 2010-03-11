/**
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include "file_utils.h"
#include <dirent.h>
#include <sys/stat.h>
#ifdef __MINGW32__
	// Mingws uses struct _stati64 and function _stati64, in place of POSIX's stat64
	#define stat64 _stati64
#endif


/**
 * Returns 1 if the given filename is a directory, 0 if not, and -1 if an error occurs (use strerror(errno) to figure out the reason).
 */
int is_directory(string filename) {
	struct stat64 s;
	int result;
	for (int i = 0; i < 10; i++) { // If stat returns EAGAIN, we'll try this 10 times
		result = stat64(filename.c_str(), &s);
		if (result == 0 || errno != EAGAIN) break;
	}
	if (result != 0) return -1;
	return S_ISDIR(s.st_mode) ? 1 : 0;
}


#ifdef _WIN32
 #define PATH_SEP "\\"
#else
 #define PATH_SEP "/"
#endif

/**
 * Returns false if the listing fails. Reason can be obtained via strerror(errno).
 */
bool list_files(string dir, vector<string>& files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) return false;
    while ((dirp = readdir(dp)) != NULL) {
        if (strcmp(dirp->d_name, ".") == 0) continue;
        if (strcmp(dirp->d_name, "..") == 0) continue;
        string new_entry = dir + PATH_SEP + dirp->d_name;
        files.push_back(new_entry);
    }
    closedir(dp);
    return true;
}

