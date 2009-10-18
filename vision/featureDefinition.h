/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2009, Stanford University
**
** FILENAME:    featureDefinition.h
** AUTHOR(S):   Ian Goodfellow <ia3n@stanford.edu>
** DESCRIPTION:
**  Storage class for a single localized fragment feature.
**
*****************************************************************************/

#pragma once

#include <cassert>
#include <iostream>
#include <vector>
#include "cv.h"
#include "cxcore.h"
#include "xmlParser/xmlParser.h"
#include "utils.h"

using namespace std;

class FeatureDictionary;

class FeatureDefinition 
{
    friend class FeatureDictionary;

 protected:
    IplImage *_template;    // (single channel) feature template
    CvRect _validRect;       // valid response region

  public:
    FeatureDefinition();
    FeatureDefinition(XMLNode& node);
    FeatureDefinition(const IplImage *t, const CvRect& rect);
    ~FeatureDefinition();
    
    bool read(XMLNode& node);

    inline const IplImage* getTemplate() { return _template; }
    inline CvRect getValidRect() { return CvRect(_validRect); }
    inline int getTemplateWidth() { return _template->width; }
    inline int getTemplateHeight() { return _template->height; }
    inline int getTemplateSize() { return _template->width * _template->height; }

    FeatureDefinition * clone() const {
      return new FeatureDefinition(_template, _validRect); 
    }
};



