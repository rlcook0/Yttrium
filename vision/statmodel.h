
#pragma once

#include "cv.h"
#include "cxcore.h"
#include "ml.h"

#include "utils.h"
#include "objects.h"
#include "template.h"

/* Classifier Type class ---------------------------------------------------------
 */
 
using namespace std;


class Metrics {
public:
    int tp;
    int tn;
    int fp;
    int fn;
    
    Metrics() { reset(); }
    
    void reset() { tp = tn = fp = fn = 0; } 
    
    void add(int guess, int truth);
    
    double precision();
    double recall();
    double truenr();
    double accuracy();
    double fscore();
    
    void out();
};
 
int stringToClassInt(string type);
string classIntToString(int type);

class Dataset {
    
    DataSet();
    
    void push(string class, int image, float *sample);
    float *get(int image, int sample);
    float get(int image, int sample, int ith);
    string get_class(int image);
    
    void recount();
    
    int num_features;
    int num_classes;
    int num_samples;
    
    vector<pair<int, vector<float *> > > data;
    
    CvMat *centers();
    CvMat *kmeans_input();
    CvMat *test_input(CvMat *clusters);
    
protected:
    
    CvMat *pre_kmeans;   //  PRE-kmeans
    CvMat *post_kmeans;  // POST-kmeans
    
};
 
class StatModel {
public:
    
    StatModel() { savename = "StatModel.dat"; name="StatModel"; model = NULL; }
    
    // load and save classifier configuration
    virtual bool load();
    virtual bool save();
    
    virtual bool load(const char *);
    virtual bool save(const char *);
    
    virtual bool setup();
    virtual bool train(Dataset *data);
    virtual bool test(Dataset *data);
    virtual bool predict();
    
    // stats
    virtual float score();
    
protected:
    
    virtual bool train_run(Dataset *data) = 0;
    virtual bool test_run(Dataset *data) = 0;
    
    Metrics scores;
    CvStatModel *model;
    char *savename;
    char *name;
    
};


class SM_SVM : StatModel {
public:
    
    SM_SVM() { savename = "svm.dat"; name="SVM"; model = NULL; model = &svm; }
    
protected:
    
    virtual bool train_run(Dataset *data);
    virtual bool test_run(Dataset *data);

    CvSVM svm;
    
};

