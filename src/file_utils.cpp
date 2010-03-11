/**
 * Copyright: 2009-2010, Konstantin Tretyakov, Pjotr Prins, Swen Laur
 * License:   The terms of use of this software and its source code are defined by the BSD license.
 */
#include "file_utils.h"
#include <dirent.h>

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

