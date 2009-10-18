/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2009, Stanford University
**
** FILENAME:    infToken.h
** AUTHOR(S):   Ian Goodfellow <ia3n@stanford.edu>
** DESCRIPTION:
**  Utilities for dealing reading in and correctly interpreting the "inf" value
**
*****************************************************************************/

#pragma once

#include <limits>
using namespace std;

template<class T, bool B>
struct parse_inf_token {
    static bool apply(const std::string& token, T& value);
};

template<class T>
struct parse_inf_token<T, true>  {
  static bool apply(const std::string& token, T& value) {
    if (token.compare("-inf") == 0) {
      value = -numeric_limits<T>::infinity();
        } else if (token.compare("inf") == 0) {
      value = numeric_limits<T>::infinity();
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
