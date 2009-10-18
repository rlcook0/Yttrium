/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    utils.cpp
** AUTHOR(S):   Stephen Gould <sgould@stanford.edu>
** DESCRIPTION:
**  Useful utility functions.
*****************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <sys/types.h>
#include <dirent.h>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "utils.h"

using namespace std;

// getTrainingFiles
TTrainingFileList getTrainingFiles(const char *root, const char *ext)
{
    TTrainingFileList fileList;
    DIR *rootdir;
    string directory;
    int len = strlen(ext);

    struct dirent *rootEntry;
    directory = string(root);
    if (*(directory.rbegin()) != '/') {
	directory = directory + string("/");
    }
    if ((rootdir = opendir(directory.c_str())) == NULL) {
	cerr << "ERROR: could not open root directory" << endl;
	exit(-1);
    }
    
    while (1) {
	// search for subdirectories under the root
	if ((rootEntry = readdir(rootdir)) == NULL) break;
	if (rootEntry->d_type != DT_DIR) continue;
	if (rootEntry->d_name[0] == '.') continue;

	string subdir = directory + string(rootEntry->d_name) + string("/");
	DIR *dir;
	struct dirent *entry;

	if ((dir = opendir(subdir.c_str())) == NULL) {
	    cerr << "WARNING: could not open subdirectory " << subdir.c_str() << endl;
	    continue;
	}
	
	fileList.classes.push_back(string(rootEntry->d_name));

	while (1) {
	    // search for files within the subdirectory
	    if ((entry = readdir(dir)) == NULL) break;
	    string filename = string(entry->d_name);
	    if ((int)filename.size() <= len) continue;
	    if (filename.substr(filename.size() - len, len) != ext) continue;

	    TTrainingFile fileDef;
	    fileDef.filename = subdir + filename;
	    fileDef.label = string(rootEntry->d_name);
	    fileList.files.push_back(fileDef);
	}

	closedir(dir);
    }

    closedir(rootdir);

    return fileList;
}

// showProgress
void showProgress(int index, int maxIndex)
{
    double p;

    if (maxIndex > 0) {
	p = (100.0 * index) / (double)maxIndex;
	cout << p << "%        \r";
	cout.flush();
    }
}
