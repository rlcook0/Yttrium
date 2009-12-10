

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
    
    num_images = num_samples = 0;
    post_kmeans = pre_kmeans = NULL;
}

int DataSet::push(int klass)
{
    assert (klass < num_classes && klass >= 0);
    data.push_back( DataSetImage(klass, vector<float *>() ) );
}

int DataSet::push(int klass, vector<float *> &fts)
{
    assert (klass < num_classes && klass >= 0);
    data.push_back( DataSetImage(klass, fts) );
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
    assert (sample < data[image].second.size());
    
    return data[image].second[sample];
}

float DataSet::get(int image, int sample, int ith) 
{
    assert (image < num_images);
    assert (sample < data[image].second.size());
    assert (ith < num_features);
    
    return data[image].second[sample];
}

int DataSet::get_class(int image)
{
    assert (image < num_images && image >= 0);
    return data[image].first;
}

int DataSet::count(int image)
{
    assert(image >= 0 && image < num_images);
    return data[i].second.size();
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
    
    for (unsigned int i = 0; i < data.size(); i++)
        printf("%d %s: %d \n", data[i].first, classIntToString(data[i].first), data[i].second.size());

    printf("\n");
}

CvMat *DataSet::cluster_samples() {
    if (samples != NULL) return samples;
    
    assert(clusters != NULL);
    
    samples = cvCreateMat(num_samples, NUM_CLUSTERS, CV_32FC1);
    
    int sample = 0;
    for (int i = 0; i < (int) data.size(); i++) {                   //  For Each Image
        for (int s = 0; s < (int) data[i].second.size(); s++) {     //      For Each Sample In that Image
            int cluster = cvGetReal1D(clusters, sample++);          //          Get The Cluster for this sample.
            int oldValue = cvGetReal2D(samples, i, cluster);        //          Get Old value for this [image][cluster]
            cvSetReal2D(samples, i, j, oldValue + 1);
        }
    }
    
    return samples;
}

CvMat *DataSet::cluster_responses()
{
    if (responses != NULL) return responses;
    assert(num_samples == data->size());
    
    responses = cvCreateMat(num_samples, 1, CV_32SC1);

    for (int i = 0; i < (int)data->size(); ++i) 
        cvSetReal1D(responses, i, data[i]->first);
    
    return respones;
}

CvMat *DataSet::kmeans_input()
{
    if (to_kmeans != NULL) return to_kmeans;
    
    to_kmeans = cvCreateMat(num_samples, SURF_SIZE, CV_32FC1);
    
    int sample = 0;
    for (int i = 0; i < (int) data.size(); i++) {                   //  For Each Class
        for (int s = 0; s < (int) data[i].second.size(); s++) {     //      For Each Sample In that Class
            for (int j = 0; j < SURF_SIZE; j++) {                   //          For Each Feature in that Sample
                cvSetReal2D(to_kmeans, sample++, j, data.get(i, s, j));   //      Set the next sample in the matrix
            }
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

bool StatModel::load() { load(savename); }
bool StatModel::save() { save(savename); }

bool StatModel::load(const char *filename) { if (model != NULL) model->load(filename); }
bool StatModel::save(const char *filename) { if (model != NULL) model->save(filename); }

bool StatModel::setup()
{
    
}

bool StatModel::train(DataSet *data)
{
    scores.reset();
    
    cout << "------------------------------------------" << endl;
    cout << "Training " << name << endl; 
    flush(cout);
    
    data->stats();
    
    train_run(data);
    
    save();
    
    cout << endl;
    score->out();
    cout << "------------------------------------------" << endl;
}


bool StatModel::test(DataSet *data)
{
    scores.reset();
    
    cout << "------------------------------------------" << endl;
    cout << "Testing " << name << endl; 
    flush(cout);
    
    data->stats();
    
    test_run(data);
    
    cout << endl;
    score->out();
    cout << "------------------------------------------" << endl;
}

int StatModel::test(CvMat *query, int truth)
{
    int guess = predict(query);
    score.add(guess, truth);
    return guess;
}

// SVM
bool SM_SVM::train_run(DataSet *data)
{
    svm.train(data->cluster_samples(), data->cluster_responses());
}

bool SM_SVM::test_run(CvMat *samples, CvMat *responses)
{
//    svm.
}

int SM_SVM::predict(CvMat *query)
{
    return svm.predict(query);
}

//    CvBoost trees[kNumObjectTypes];

