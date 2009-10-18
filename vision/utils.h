/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    utils.h
** AUTHOR(S):   Stephen Gould <sgould@stanford.edu>
** DESCRIPTION:
**  Useful utility functions.
*****************************************************************************/

#pragma once

#include <vector>
#include <sstream>
#include <limits>

// to_string
// convert a basic data type to a string
template<class T>
std::string to_tring(const T& v)
{
    std::stringstream s;
    s << v;
    return s.str();
}

// getTrainingFiles
// Given a directory, looks for all *.jpg files within one directory level
// below the root and labels each with the name of the subdirectory, e.g.
// all files in data/mug/*.jpg would be labeled as "mug" given the root
// directory "data".
//
typedef struct _TTrainingFile {
    std::string filename;
    std::string label;
} TTrainingFile;

typedef struct _TTrainingFileList {
    std::vector<TTrainingFile> files;
    std::vector<std::string> classes;
} TTrainingFileList;
    
TTrainingFileList getTrainingFiles(const char *root, const char *ext);

// showProgress
// Shows % complete on standard output
void showProgress(int index, int maxIndex);


// utilities for dealing with reading in and correctly interpreting
// the "inf" token
template<class T, bool B>
struct parse_inf_token {
    static bool apply(const std::string& token, T& value);
};

template<class T>
struct parse_inf_token<T, true>  {
  static bool apply(const std::string& token, T& value) {
    if (token.compare("-inf") == 0) {
      value = -std::numeric_limits<T>::infinity();
        } else if (token.compare("inf") == 0) {
      value = std::numeric_limits<T>::infinity();
    } else {
      return false;
    }
    
    return true;
  }
};

template<class T>
struct parse_inf_token<T, false>  {
    static bool apply(const std::string& token, T& value) {
        return false;
    }
};


