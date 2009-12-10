
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

typedef pair<int, vector<float *> > DataSetImage;
typedef vector<DataSetImage>  DataSetVector;
typedef vector<DataSetImage *> DataSetStarVector;

class DataSet {
    
    DataSet();
    
    
    int push(int klass);
    int push(int klass, vector<float *> &fts);
    int push(int image, int klass, float *sample);
    
    float *get(int image, int sample);
    float  get(int image, int sample, int ith);
    
    int count(int image);
    
    void recount();
    
    int num_features;
    int num_classes;
    int num_samples;
    int num_images;
    
    DataSetVector data;
    
    CvMat *kmeans_input();

    CvMat *cluster_responses();
    CvMat *cluster_samples();
        
    static CvMat *clusters;
    static CvMat *centers;
    
    
    CvMat *to_kmeans;   //  PRE-kmeans

    CvMat *samples;     // POST-kmeans
    CvMat *responses;
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
    virtual bool train(DataSet *data);
    virtual bool test(DataSet *data);
    virtual int  test(CvMat *query, int truth);
    
    // stats
    virtual float score();
    
    Metrics scores;
    
protected:
    
    virtual bool train_run(DataSet *data) = 0;
    virtual bool test_run(DataSet *data) = 0;
    virtual int predict(CvMat *query) = 0;
    
    CvStatModel *model;
    char *savename;
    char *name;
    
};


class SM_SVM : StatModel {
public:
    
    SM_SVM() { savename = "svm.dat"; name="SVM"; model = NULL; model = &svm; }
    
protected:
    
    virtual bool train_run(CvMat *data);
    virtual bool test_run(CvMat *data);

    CvSVM svm;
    
};

