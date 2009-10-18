/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2009, Stanford University
**
** FILENAME:    featureDictionary.h
** AUTHOR(S):   Ian Goodfellow <ia3n@stanford.edu>
** DESCRIPTION:
**  Stores a collection of fragment-based localized features.
**
*****************************************************************************/

#pragma once

#include <cassert>
#include <iostream>
#include <vector>

#include "cv.h"
#include "cxcore.h"
#include "xmlParser/xmlParser.h"
#include "featureDefinition.h"

using namespace std;

class FeatureDictionary
{
 protected:
     vector<FeatureDefinition * > _entries;
     CvSize _windowSize;

  public:
    FeatureDictionary(unsigned width = 0, unsigned height = 0);
    FeatureDictionary(const FeatureDictionary& d);
    ~FeatureDictionary();

    inline unsigned numFeatures() const { return _entries.size(); }
    FeatureDefinition * getFeature(unsigned i) { return _entries[i]; }

    bool load(const char *filename);
    bool load(XMLNode &root);

    inline unsigned windowWidth() const { return _windowSize.width; }
    inline unsigned windowHeight() const { return _windowSize.height; }    

    // Constructs a huge image with all the features in it.
    IplImage *visualizeDictionary() const;
    IplImage *visualizeFeature(unsigned index, IplImage *image = NULL) const;
};

