

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

void Metrics::out() 
{
   printf("tp: %d tn: %d fp: %d fn: %d -- prec: %lf rec: %lf tnr: %lf acc: %lf fscore: %lf \n",
       tp, tn, fp, fn, precision(), recall(), truenr(), accuracy(), fscore());
}

DataSet::DataSet()
{
    num_classes = kNumObjectTypes;
    num_features = SURF_SIZE;
    
    num_images = num_features = num_
    post_kmeans = pre_kmeans = NULL;
}

void  DataSet::push(int image, float *sample)
{
    assert (image < data.size() && image >= 0);
    
    for (unsigned int i = 0; i < data.size(); i++)
    {
        if (data[i].first == klass)
        {
            data[i].second.push_back(feature);
            num_samples++;
            return;
        }
    }
    
    printf("DataSet pushed feature to nonexistant class: %d \n", klass);
    assert(false);
    
}

float *DataSet::get(int image, int sample)
{
    assert (klass < kNumObjectTypes && klass >= 0);
    assert (sample < num_samples);
    
    for (unsigned int i = 0; i < data.size(); i++)
        if (data[i].first == klass)
            return data[i].second[sample];
    
    printf("DataSet getting nonexistant sample: %d or image: %d \n", sample, klass);
    stats();
    assert(false);
}

float DataSet::get(int image, int sample, int ith) 
{
    assert (klass < kNumObjectTypes && klass >= 0);
    assert (sample < num_samples);
    assert (ith < num_feature);
    
    for (unsigned int i = 0; i < data.size(); i++)
        if (data[i].first == klass)
            return data[i].second[sample][ith];
    
    printf("DataSet getting nonexistant feature: %d sample: %d or image: %d \n", ith, sample, klass);
    stats();
    assert(false);
}

void  DataSet::recount() 
{
    num_classes = num_features = num_samples = 0;
    
    num_classes = data.size();
    num_features = SURF_SIZE;
    
    for (unsigned int i = 0; i < data.size(); i++)
        num_samples += data[i].second.size();
        
    return;
}


void DataSet::stats()
{
    recount();
    printf("DataSet -- classes: %d samples: %d \n", num_classes, num_samples );
    
    for (unsigned int i = 0; i < data.size(); i++)
        printf("%d %s: %d \n", data[i].first, classIntToString(data[i].first), data[i].second.size());

    printf("\n");
}


CvMat *DataSet::test_input(CvMat *clusters) {
    if (post_kmeans != NULL) return post_kmeans;
    
    assert(clusters != NULL);
    
    post_kmeans = cvCreateMat(num_samples, NUM_CLUSTERS, CV_32FC1);
    
    int sample = 0;
    for (int i = 0; i < (int) data.size(); i++) {                   //  For Each Class
        for (int s = 0; s < (int) data[i].second.size(); s++) {     //      For Each Sample In that Class
            int cluster = cvGetReal1D(clusters, sample++);          //          Get The Cluster for this sample.
            int oldValue = post_kmeans.get(i, cluster);             //          Get Old value for this [sample][cluster]
            post_kmeans->set(i, cluster, oldValue + 1);             //          Save Increment
            cvSetReal2D(post_kmeans, i, j, pt[j]);
        }
    }
    
    return post_kmeans->matrix;
}

CvMat *DataSet::kmeans_input()
{
    if (pre_kmeans != NULL) return pre_kmeans;
    
    pre_kmeans = cvCreateMat(num_samples, SURF_SIZE, CV_32FC1);
    
    int sample = 0;
    for (int i = 0; i < (int) data.size(); i++) {                   //  For Each Class
        for (int s = 0; s < (int) data[i].second.size(); s++) {     //      For Each Sample In that Class
            for (int j = 0; j < SURF_SIZE; j++) {                   //          For Each Feature in that Sample
                cvSetReal2D(pre_kmeans, sample++, j, data.get(i, s, j));   //      Set the next sample in the matrix
            }
        }
    }
    
    return pre_kmeans->matrix;
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

bool StatModel::load() { load(savename); }
bool StatModel::save() { save(savename); }

bool StatModel::load(const char *filename) { if (model != NULL) model->load(filename); }
bool StatModel::save(const char *filename) { if (model != NULL) model->save(filename); }

bool StatModel::setup()
{
    
}

bool StatModel::train(Dataset *data)
{
    scores.reset();
    
    cout << "------------------------------------------" << endl;
    cout << "Training " << name << endl; 
    flush(cout);
    
    data->stats();
    
    train_run(Dataset *data);
    
    cout << endl;
    score->out();
    cout << "------------------------------------------" << endl;
}


bool StatModel::test(Dataset *data)
{
    scores.reset();
    
    cout << "------------------------------------------" << endl;
    cout << "Testing " << name << endl; 
    flush(cout);
    
    data->stats();
    
    test_run(Dataset *data);
    
    cout << endl;
    score->out();
    cout << "------------------------------------------" << endl;
}

bool SM_SVM::train_run()
{
    
}


