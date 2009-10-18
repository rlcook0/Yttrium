/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2009, Stanford University
**
** FILENAME:    capture.h
** AUTHOR(S):   Ian Goofellow <ia3n@stanford.edu>
** DESCRIPTION:
**  Captures video from frames saves in a directory
**
*****************************************************************************/

#pragma once

#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

#if defined(_WIN32)||defined(WIN32)||defined(__WIN32__)
#include "win32/dirent.h"
#include <windows.h>
#else
#include <dirent.h>
#endif

class Capture
{
 public:
  Capture(const char * directory )
    {
      //based on svlDirectoryListing by Stephen Gould
      const char extension[] = { '.','j','p','g','\0' };
      bool bIncludeDir = true;
      bool bIncludeExt = true;

      DIR *dir = opendir(directory);
      if (dir == NULL) {
        cerr <<  "could not open directory " << directory;
	exit(-1);
      }
      
      string prefix;
      if (bIncludeDir) {
        prefix = string(directory) + string("/");
      }
      
      int extLength = 0;
      if (extension != NULL) {
        extLength = strlen(extension);
      }
      
      struct dirent *e = readdir(dir);
      while (e != NULL) {
        // skip . and ..
        if (!strncmp(e->d_name, ".", 1) || !(strncmp(e->d_name, "..", 2))) {
	  e = readdir(dir);
	  continue;
        }
	
        bool bIncludeFile = false;
        if (extension == NULL) {
	  bIncludeFile = true;
        } else {
	  const char *p = strstr(e->d_name, extension);
	  if ((p != NULL) && (*(p + strlen(extension)) == '\0')) {
	    bIncludeFile = true;
	  }
        }
	
        if (bIncludeFile) {
	  string filename = string(e->d_name);
            if (!bIncludeExt) {
	      filename.erase(filename.length() - extLength);
            }
            _filenames.push_back(prefix + filename);
        }
	
        e = readdir(dir);
      }    
      closedir(dir);
      
      sort(_filenames.begin(), _filenames.end());
      
      _idx = 0;
    }
  
  IplImage * queryFrame()
  {
    if (_idx < _filenames.size())
      return cvLoadImage(_filenames[_idx++].c_str());
    return NULL;
  }

 private:

  unsigned _idx;
  vector<string> _filenames;



};
