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

#include "logreg.h"
#include "template.h"
#include "statmodel.h"

/* Classifier class ---------------------------------------------------------
 */


using namespace std;


typedef struct FoundObject {
    CObject object; 
    double score;
} FoundObject;


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
    
public:
    // constructors
    Classifier();
    
    // destructor
    virtual ~Classifier();
    
    bool loadState();

    // run the classifier over a single frame
    void optical_flow(const IplImage *frame, double *xD, double *yD);
    bool run_boxscan(IplImage *dst, vector<int> &cluster, vector<CvSURFPoint> &keypts, vector<float *> &pts, vector<FoundObject> &newObjects, const CObjectList *oldObjects);
    bool run(const IplImage *frame, CObjectList *objects, bool scored);
    bool showRect(IplImage *image, CObject *rect, const vector<CvSURFPoint> *pts = NULL);
    
    // extract the classifier features
    virtual bool extract(TTrainingFileList&);
        
    // train the classifier using given set of files
    virtual bool train(); 
    
    bool test(DataSet *data);
    
    virtual bool kmeans(DataSet *data);   
    
    int best_cluster(CvMat *centers, float *vars);
    
    // Settings
    bool kmeans_load;
    bool kmeans_save;
    
    bool load_all;
    bool save_all;
    
    bool test_on;
    
    int num_clusters;
    int max_others;
    int test_to_train;
    
    SM_Bayes bayes;
    SM_SVM svm;
    SM_RTrees rtrees;
    SM_BTrees btrees;
    //StatModel rtree;
    ///StatModel trees;

    IplImage *prevFrame;

private:
    
};

