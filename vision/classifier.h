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
#include "ml.h"

#include "utils.h"
#include "objects.h"
#include "featureDictionary.h"

#include "logreg.h"
#include "template.h"
#include "statmodel.h"

/* Classifier class ---------------------------------------------------------
 */

#define NUM_CLUSTERS    1000
#define MIN_IPOINTS     15
#define SURF_SIZE       128
 
enum ObjectTypes {
    kClock = 0,
    kMug,
    kKeyboard,
    kStapler,
    kScissors,
    kOther,
    kNumObjectTypes
};

using namespace std;


class Ipoint;
struct CvFeatureTree;

class Classifier {
protected:
    CvRNG rng;
    CvMemStorage* storage;
    
    // CS221 TO DO: ADD YOUR MEMBER VARIABLES HERE
    DataSet set_all;
    DataSet set_test;
    DataSet set_train;
        
    CvMat* centers;
    
//    SM_Bayes bayes;
    SM_SVM svm;
    //StatModel rtree;
    ///StatModel trees;
    
public:
    // constructors
    Classifier();
    
    // destructor
    virtual ~Classifier();

    // run the classifier over a single frame
    virtual bool run(const IplImage *, CObjectList *, bool);
    virtual bool run_boxscan(IplImage *, vector<int> &, vector<CvSURFPoint> &, vector<feat> &);
    
    // extract the classifier features
    virtual bool extract(TTrainingFileList&);
        
    // train the classifier using given set of files
    virtual bool train();    
    
    // Settings
    bool kmeans_load;
    bool kmeans_save;
    
    bool bayes_on;
    bool svm_on;
    bool rtree_on;
    bool trees_on;
    
    bool test_on;
    
    int num_clusters;
    int max_others;
    

private:
    
    bool showRect(IplImage *, CObject *, const vector<CvSURFPoint> *);
};

