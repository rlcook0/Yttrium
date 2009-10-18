/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2009, Stanford University
**
** FILENAME:    featureDefinition.cpp
** AUTHOR(S):   Ian Goodfellow <ia3n@stanford.edu>
** DESCRIPTION:
**  See featureDefinition.h
**
*****************************************************************************/

#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <limits>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "featureDefinition.h"

using namespace std;

// Conversion from a string (used to correctly process 'inf' values if
// those exist)
template<class T>
static int parseString(const std::string& str, std::vector<T>& v)
{
  std::stringstream buffer;
  T data;
  int count;
  
  buffer << str;
  
  count = 0;
  while (1) {
    int lastPosition = buffer.tellg();
    buffer >> data;
    if (buffer.fail()) {
      // try to parse special token
      buffer.clear();
      buffer.seekg(lastPosition, ios::beg);
      string token;
      buffer >> token;            
      if (!parse_inf_token<T, numeric_limits<T>::has_infinity>::apply(token, data)) {
	break;
      }
    }
    v.push_back(data);
    count++;
    
    if (buffer.eof()) break;
  }
	
  return count;
}


// FeatureDefinition class --------------------------------------------------

FeatureDefinition::FeatureDefinition() :
  _template(NULL) 
{
    _validRect = cvRect(0, 0, 0, 0);
}

FeatureDefinition::FeatureDefinition(XMLNode& node) :
  _template(NULL)
{
    _validRect = cvRect(0, 0, 0, 0);
    read(node);
}

FeatureDefinition::FeatureDefinition(const IplImage *t, const CvRect& rect)
{
  if (t != NULL) {
        _template = cvCloneImage(t);
    } else {
        _template = NULL;
    }
    _validRect.x = rect.x;
    _validRect.y = rect.y;
    _validRect.width = rect.width;
    _validRect.height = rect.height;

 }

FeatureDefinition::~FeatureDefinition()
{
    if (_template != NULL) {
        cvReleaseImage(&_template);
	
	_template = NULL;
    }
}

bool FeatureDefinition::read(XMLNode &node)
{
  assert(!node.isEmpty());
  if (_template != NULL) {
    cvReleaseImage(&_template);
  }
    
  // Read valid region
  if ( node.getAttribute("validRect") ) {
    vector<int> v;
    parseString<int>(node.getAttribute("validRect"), v);
    assert(v.size() == 4);
    _validRect = cvRect(v[0], v[1], v[2], v[3]);
  } else {
    _validRect = cvRect(0, 0, 0, 0);
  }
    
  // Read feature size
  int w = 0;
  int h = 0;
  if (node.getAttribute("size")) {
    vector<int> v;
    parseString<int>(node.getAttribute("size"), v);
    assert(v.size() == 2);
    w = v[0]; h = v[1];
  }
  
  // Read feature data
  if ( w > 0 && h > 0 ) {
    _template = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
    vector<int> v;
    parseString<int>(node.getText(), v);
    assert(v.size() == (unsigned)(w * h));
    unsigned n = 0;
    for (unsigned y = 0; y < (unsigned)_template->height; y++) {
      uchar *p = &CV_IMAGE_ELEM(_template, uchar, y, 0);
      for (unsigned x = 0; x < (unsigned)_template->width; x++) {
	p[x] = v[n++];
      }
    }    
  }
  return true;
}




