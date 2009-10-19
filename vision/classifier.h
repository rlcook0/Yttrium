/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    classifier.h
** AUTHOR(S):   Stephen Gould <sgould@stanford.edu>
**              Ian Goodfellow <ia3n@stanford.edu>
** DESCRIPTION:
**  The Classifier class runs all the object classifiers. The loadState()
**  member function is called during initialization with the filename of the
**  configuration file. The run() member function is then called on each video
**  frame returning a list of objects found.
**  During training, the train() method is called with a list of files on
**  which to train the classifier. The saveState() function is called once
**  training has finished to save the new configuration.
**
** CS221 TO DO:
**  Implement the loadState(), saveState(), run(), and train() member
**  functions.
*****************************************************************************/

#pragma once

#include "cv.h"
#include "cxcore.h"

#include "utils.h"
#include "objects.h"
#include "featureDictionary.h"

/* Classifier class ---------------------------------------------------------
 */
class Classifier {
protected:
    CvRNG rng;

    // CS221 TO DO: ADD YOUR MEMBER VARIABLES HERE
    
public:
    // constructors
    Classifier();
    
    // destructor
    virtual ~Classifier();

    // load and save classifier configuration
    virtual bool loadState(const char *);
    virtual bool saveState(const char *);

    // run the classifier over a single frame
    virtual bool run(const IplImage *, CObjectList *, bool);
        
    // train the classifier using given set of files
    virtual bool train(TTrainingFileList&);

private:

    FeatureDictionary _features;



    // CS221 TO DO: ADD YOUR MEMBER FUNCTIONS HERE
};
