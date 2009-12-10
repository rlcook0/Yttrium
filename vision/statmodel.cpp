

#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>
 
#include "statmodel.h"

// Classifier class ---------------------------------------------------------

void Metrics::add(int guess, int truth) 
{
   if      (guess == kOther && truth == kOther) tn++;
   else if (guess == kOther && truth != kOther) fn++;
   else if (guess != kOther && truth != guess ) fp++;
   else if (guess != kOther && truth == guess ) tp++;
}
   
double Metrics::precision() { return (double) tp / (double)(tp + fp); }
double Metrics::recall()    { return (double) tp / (double)(tp + fn); }
double Metrics::truenr()    { return (double) tn / (double)(tn + fp); }
double Metrics::accuracy()  { return (double) (tp + tn) / (double)(tn + tp + fn + fp); }
double Metrics::fscore()    { return 2 * precision() * recall() / (precision() + recall()); }

void Metrics::out(const char *msg) 
{
   printf("%s tp: %d tn: %d fp: %d fn: %d -- prec: %lf rec: %lf tnr: %lf acc: %lf fscore: %lf \n",
       msg, tp, tn, fp, fn, precision(), recall(), truenr(), accuracy(), fscore());
}

DataSet::DataSet()
{
    num_classes = kNumObjectTypes;
    num_features = SURF_SIZE;
    
    num_images = num_samples = 0;
    to_kmeans = responses = samples = NULL;
    clusters = NULL;
}

int DataSet::push(int klass)
{
    assert (klass < num_classes && klass >= 0);
    data.push_back( DataSetImage(klass, vector<float *>() ) );
    return num_images++;
}

int DataSet::push(int klass, vector<float *> &fts)
{
    assert (klass < num_classes && klass >= 0);
    data.push_back( DataSetImage(klass, fts) );
    num_samples += fts.size();
    return num_images++;
}

int  DataSet::push(int image, int klass, float *sample)
{
    assert (klass < num_classes && klass >= 0);
    assert (image >= 0 && image < num_images);
    assert (sample != NULL);
    
    assert (data[image].first == klass);
    data[image].second.push_back(sample);
    
    num_samples++;
    return data[image].second.size() - 1;
    
}

float *DataSet::get(int image, int sample)
{
    assert (image < num_images);
    assert (sample < num_samples);
    
    return data[image].second[sample];
}

float DataSet::get(int image, int sample, int ith) 
{
    assert (image < num_images);
    assert (sample < num_samples);
    assert (ith < num_features);
    
    return data[image].second[sample][ith];
}

int DataSet::get_class(int image)
{
    assert (image < num_images && image >= 0);
    return data[image].first;
}

int DataSet::count(int image)
{
    assert(image >= 0 && image < num_images);
    return (int)data[image].second.size();
}

void  DataSet::recount() 
{
    
    num_classes = kNumObjectTypes;
    num_features = SURF_SIZE;
    num_images = data.size();
    num_samples = 0;
    
    for (unsigned int i = 0; i < data.size(); i++)
        num_samples += data[i].second.size();
        
    return;
}


void DataSet::stats()
{
    recount();
    printf("DataSet -- classes: %d images: %d samples: %d fts: %d \n", num_classes, num_images, num_samples, num_features );
    
    int counts[kNumObjectTypes][2] = { {0} };
    for (int i = 0; i < num_images; i++)
    {
        counts[ get_class(i) ][0] += count(i);
        counts[ get_class(i) ][1]++;
    }
    
    for (int i = 0; i < num_classes; i++)
        printf("%d %s: %d images, %d samples \n", i, classIntToString(i).c_str(), counts[i][1], counts[i][0]);

    printf("\n");
}

CvMat *DataSet::cluster_samples() {
    if (samples != NULL) return samples;
    
    assert(clusters != NULL);
    
    samples = cvCreateMat(num_samples, centers->rows, CV_32FC1);
    
    int sample = 0;
    for (int i = 0; i < (int) data.size(); i++) {                   //  For Each Image
        for (int s = 0; s < (int) data[i].second.size(); s++) {     //      For Each Sample In that Image
            int cluster = cvGetReal1D(clusters, sample++);          //          Get The Cluster for this sample.
            int oldValue = cvGetReal2D(samples, i, cluster);        //          Get Old value for this [image][cluster]
            cvSetReal2D(samples, i, cluster, oldValue + 1);         //          Set Incremented Value
        }
    }
    
    return samples;
}

CvMat *DataSet::cluster_responses()
{
    if (responses != NULL) return responses;
    assert(num_images == (int)data.size());
    
    responses = cvCreateMat(num_samples, 1, CV_32SC1);

    for (int i = 0; i < (int)data.size(); ++i) 
        cvSetReal1D(responses, i, data[i].first);
    
    return responses;
}

CvMat *DataSet::kmeans_input()
{
    if (to_kmeans != NULL) return to_kmeans;
    
    to_kmeans = cvCreateMat(num_samples, SURF_SIZE, CV_32FC1);
    
    int sample = 0;
    for (int i = 0; i < (int) data.size(); i++) {                   //  For Each Class
        for (int s = 0; s < (int) data[i].second.size(); s++) {     //      For Each Sample In that Class
            for (int j = 0; j < SURF_SIZE; j++) {                   //          For Each Feature in that Sample
                cvSetReal2D(to_kmeans, sample, j, get(i, s, j));  //          Set the next sample in the matrix
            }
            sample++;
        }
    }
    
    return to_kmeans;
}


int stringToClassInt(string type) 
{
    if (type == "other")    return kOther;
    if (type == "mug")      return kMug;
    if (type == "keyboard") return kKeyboard;
    if (type == "clock")    return kClock;
    if (type == "stapler")  return kStapler;
    if (type == "scissors") return kScissors;
    
    return -1;
}

string classIntToString(int type) 
{
    if (type == kOther)     return "other";
    if (type == kMug)       return "mug";
    if (type == kKeyboard)  return "keyboard";
    if (type == kClock)     return "clock";
    if (type == kStapler)   return "stapler";
    if (type == kScissors)  return "scissors";
    
    return "error";
}

bool StatModel::load() { return load(savename.c_str()); }
bool StatModel::save() { return save(savename.c_str()); }

bool StatModel::load(const char *filename) { if (model != NULL) model->load(filename); return model != NULL; }
bool StatModel::save(const char *filename) { if (model != NULL) model->save(filename); return model != NULL; }

bool StatModel::setup()
{
    if (!on) return false;
    
    return true;
}

bool StatModel::train(DataSet *data)
{
    if (!on) return false;
    
    scores.reset();
    
    cout << "------------------------------------------" << endl;
    cout << "\t\tTraining " << name << endl; 
    flush(cout);
    
//    data->stats();
    
    train_run(data);
    
    if (save_on) save();
    
    cout << endl;
    scores.out();
    cout << "------------------------------------------" << endl;
    return true;
}


bool StatModel::test(DataSet *data)
{
    if (!on) return false;
    
    scores.reset();
    
    cout << "------------------------------------------" << endl;
    cout << "\t\tTesting " << name << endl; 
    flush(cout);
    
//    data->stats();
    
    test_run(data);
    
    cout << endl;
    scores.out();
    cout << "------------------------------------------" << endl;
    return true;
}

int StatModel::test(CvMat *query, int truth)
{
    if (!on) return false;
    
    int guess = predict(query);
    scores.add(guess, truth);
    return guess;
}

// SVM
bool SM_SVM::train_run(DataSet *data)
{
    svm.train(data->cluster_samples(), data->cluster_responses());
    return true;
}

bool SM_SVM::test_run(DataSet *data)
{
//    svm.    
    return true;
}

int SM_SVM::predict(CvMat *query)
{
    return svm.predict(query);
}

//    CvBoost trees[kNumObjectTypes];



// SVM
bool SM_RTrees::train_run(DataSet *data)
{
    CvRTParams params( 
        5,                                 //Max depth 
        2,                                 //Min sample count 
        0,                                 //Regression accuracy 
        false,                              //Use Surrogates 
        10,                                 //Max Categories 
        NULL,                               //Priors  
        false,                              //Calculate Variables variance 
        0,                                  //Nactive Vars 
        500,                                //Max Tree Count 
        0.1,                            //Accuracy to archive 
        CV_TERMCRIT_EPS|CV_TERMCRIT_ITER    //Criteria to stop algorithm 
    );
    
    CvMat *desc = data->cluster_samples();
    CvMat *var_type = cvCreateMat(desc->cols + 1, 1, CV_8U);
    cvSet(var_type, cvScalarAll(CV_VAR_NUMERICAL));
    cvSet2D(var_type, desc->cols, 0, cvScalar(CV_VAR_CATEGORICAL));
    
    rtrees.train(
        desc, 
        CV_ROW_SAMPLE,
        data->cluster_responses(),
        0,
        0,
        var_type,
        0,
        params
    );

    return true;
}

bool SM_RTrees::test_run(DataSet *data)
{
//    svm.    
    return true;
}

int SM_RTrees::predict(CvMat *query)
{
    return rtrees.predict(query);
}

//    CvBoost trees[kNumObjectTypes];




