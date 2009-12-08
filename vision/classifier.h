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

/* Classifier class ---------------------------------------------------------
 */

#define NUM_CLUSTERS    500
#define MIN_IPOINTS     5
 
enum ObjectTypes {
    kClock = 1,
    kMug,
    kKeyboard,
    kOther,
    kStapler,
    kScissors,
};

using namespace std;

class Ipoint;
struct CvFeatureTree;

class Classifier {
protected:
    CvRNG rng;

    // CS221 TO DO: ADD YOUR MEMBER VARIABLES HERE
    map< string, vector<Ipoint> > surfFeatures;
    vector<pair<string, vector<Ipoint> > > allImages;
    
    CvFeatureTree *surfFT, *centersFT;
    
    vector< pair<string, int> > surfThresh;
    map<string, int> surfTotal;
    
    
    int surfTotalIpoints;
    
    string indexToClass(int index);
    int indexToClassInt(int index);
    string classIntToString(int type);
    int stringToClassInt(string type);
    
    CvNormalBayesClassifier bayes;
    CvKNearest knn;
    CvSVM svm;

    CvMat* centers;
public:
    // constructors
    Classifier();
    
    // destructor
    virtual ~Classifier();

    // load and save classifier configuration
    virtual bool loadState(const char *);
    virtual bool saveState(const char *);

    // load and save classifier configuration    
    virtual bool loadSURFFile(const char *);
    virtual bool saveSURFFile(const char *);
    
    virtual bool setupKDTree();
    
    // run the classifier over a single frame
    virtual bool run(const IplImage *, CObjectList *, bool);
    virtual bool run_boxscan(IplImage *, vector<int> &, vector<Ipoint> &);
    
    // extract the classifier features
    virtual bool extract(TTrainingFileList&, const char *);
        
    // train the classifier using given set of files
    virtual bool train(TTrainingFileList&, const char *);
        
    virtual bool train_kmeans(CvMat *);
    virtual bool train_bayes(CvMat *, CvMat *);
    virtual bool train_svm(CvMat *, CvMat *);
    virtual bool train_knn(CvMat *, CvMat *);
    virtual bool train_test();

private:
    
    bool showRect(IplImage *, CObject *, const vector<Ipoint> *);
};

