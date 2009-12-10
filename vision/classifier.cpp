/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME: classifier.cpp
** AUTHOR(S): Stephen Gould <sgould@stanford.edu>
** Ian Goodfellow <ia3n@stanford.edu>
** DESCRIPTION:
** See classifier.h
**
*****************************************************************************/
 
#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <sys/stat.h>
 
#include "highgui.h"

#include "surf.h"
#include "surflib.h"

#include "cv.h"
#include "cxcore.h"
 
#include "classifier.h"
#include "logreg.h"

// Classifier class ---------------------------------------------------------
 
// default constructor
Classifier::Classifier()
{
    // initalize the random number generator (for sample code)
    rng = cvRNG(-1);
 
    storage = cvCreateMemStorage(0);
 
    max_others = 2000;
    num_clusters = 100;
    kmeans_load = false;
    kmeans_save = true;
    
    test_on = true;
    
}
    
// destructor
Classifier::~Classifier()
{
}
 

CvMat *normalize(const CvMat* vector) {
    CvMat *norm = cvCloneMat(vector);
    double normVal = cvNorm(vector);
    cvScale(vector, norm, 1 / normVal);
    
    return norm;
}


// TESTING ONLY
bool Classifier::showRect(IplImage *image, CObject *rect, const vector<CvSURFPoint> *pts = NULL) {
    //char *WINDOW_NAME = "test";
    // CvFont font;
    // 
    // IplImage *frameCopy = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
    // cvConvertImage(image, frameCopy);
    //     
    // cvNamedWindow("test", CV_WINDOW_AUTOSIZE);
    // cvInitFont(&font, CV_FONT_VECTOR0, 0.75, 0.75, 0.0f, 1, CV_AA);
    // 
    // if (pts != NULL) {
    //     CvScalar color = CV_RGB(255, 255, 255);
    //     for (int i = 0; i < (int)pts->size(); ++i) {
    //         //printf("(%f %f)->(%d %d) ", (*pts)[i].x, (*pts)[i].y, frameCopy->width, frameCopy->height);
    //         cvRectangle(frameCopy, cvPoint((*pts)[i].pt.x, (*pts)[i].pt.y), 
    //             cvPoint((*pts)[i].pt.x + 3, (*pts)[i].pt.y + 3), color);
    //     }
    // }
    // 
    // rect->draw(frameCopy, CV_RGB(255, 0, 255), &font);
    //     
    // cvShowImage("test", frameCopy);
    // cvReleaseImage(&frameCopy);
    // 
    // return cvWaitKey(10) != -1;
}
// run
// Runs the classifier over the given frame and returns a list of
// objects found (and their location).
// you only need to populate "objects" if "scored" is true. if you do
// populate "objects" when "scored" is false, it will not affect your 
// score but it may be helpful to you for debugging (to visualize your
// results on more frames)
bool Classifier::run(const IplImage *frame, CObjectList *objects, bool scored)
{
    // if (!scored)
    //       return true;
    //   
    //   assert(centers != NULL);
    //   
    //   cout << "--------------------------------------" << endl;
    //   cout << "\t\tRun" << endl;
    //   
    //   assert((frame != NULL) && (objects != NULL));
    //   
    //   printf("Let's go!\n");
    //   
    //   // Convert to grayscale.
    //   IplImage *gray  = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
    //   cvCvtColor(frame, gray, CV_BGR2GRAY);
    //   
    //   // Resize by half first, as per the handout.
    //   double scale = 2.0;
    //   IplImage *dst = cvCreateImage(cvSize(gray->width  / scale, gray->height  / scale), gray->depth,  gray->nChannels);
    //   cvResize(gray, dst);
    // 
    //   printf("About to do SURF\n");
    //   CvSeq *keypoints = 0, *descriptors = 0;
    //   CvSURFParams params = cvSURFParams(300, true);
    //   cvExtractSURF(dst, 0, &keypoints, &descriptors, storage, params);
    //   
    //   if (descriptors->total == 0) return false;
    //   
    //   vector<float> desc;
    //   desc.resize(descriptors->total * descriptors->elem_size/sizeof(float));
    //   cvCvtSeqToArray(descriptors, &desc[0]);
    //   
    //   vector<CvSURFPoint> keypts;
    //   keypts.resize(keypoints->total);
    //   cvCvtSeqToArray(keypoints, &keypts[0]);
    //   
    //   vector<feat> features;
    //   int where = 0;
    //   for (int pt = 0; pt < keypoints->total; ++pt) {
    //       float *f = new float[128];
    //       for (int j = 0; j < 128; ++j) {
    //           f[j] = desc[where];
    //       }
    //       features.push_back(f);
    //   }
    //   printf("Done SURF\n");
    // 
    //   printf("Clustering...\n");
    //   //cout << centers;
    //   //printf("\n");
    //   //printf("%d %d\n", centers->rows, centers->cols);
    //   vector<int> cluster(features.size());
    //   for (int i = 0; i < (int)features.size(); ++i) {
    //       double min_distance = DBL_MAX;
    //       int min_index = -1;
    //       for (int c = 0; c < NUM_CLUSTERS; ++c) {
    //           float dist = 0;
    //           for (int j = 0; j < SURF_SIZE; ++j) {
    //               dist += pow(features[i][j] - cvGetReal2D(centers, c, j), 2);
    //           }
    //   
    //           if (dist < min_distance) {
    //               min_index = c;
    //               min_distance = dist;
    //           }
    //       }
    // 
    //       cluster[i] = min_index;
    //   }
    //   printf("Done clustering...\n");
    //   
    //   run_boxscan(dst, cluster, keypts, features);
    // 
    //   cvReleaseImage(&gray);
    //   return true;
}


// bool Classifier::run_boxscan(IplImage *dst, vector<int> &cluster, vector<CvSURFPoint> &keypts, vector<feat> &pts)
// {
//     float scale = 2.0f;
//     int maxWidth = dst->width;
//     int maxHeight = dst->height;
//     
//     int numLayers = 10;
//     while(numLayers-- > 0) {
//         for (int x = 0; x < maxWidth - 32*scale; x += 8) {
//             for (int y = 0; y < maxHeight - 32*scale; y += 8) {
//                 vector<int> newpts;
//                 
//                 for (int i = 0; i < (int)pts.size(); ++i) {
//                     if (keypts[i].pt.x >= x && keypts[i].pt.x < x + 32*scale && keypts[i].pt.y >= y && keypts[i].pt.y < y + 32*scale)
//                         newpts.push_back(i);
//                 }
//                 
//                 if (newpts.size() < MIN_IPOINTS) continue;
// 
//                 CvMat *query = cvCreateMat(1, NUM_CLUSTERS, CV_32FC1);
//                 cvSet(query, cvScalar(0));
//                 for (int row = 0; row < (int)newpts.size(); ++row) {
//                     int idx = newpts[row];
//                     int cluster_idx = cluster[idx];
// 
//                     int oldVal = cvGetReal2D(query, 0, cluster_idx);
//                     cvSetReal2D(query, 0, cluster_idx, oldVal+1);
//                 }
//         
//                 int klass, klass2, klass3, klass4, klass5;
//                 double scores[kNumObjectTypes - 1];
//                 
//                 if (BAYES_ON) klass = bayes.predict(query);
//                 if (SVM_ON)   klass2 = svm.predict(query);
//                 if (KNN_ON)   klass3 = knn.find_nearest(query, 5);
//                 if (RTREE_ON) klass4 = rtree.predict(query);
//                 if (ALL_TREES_ON) {
//                     for (int klass = 0; klass < kNumObjectTypes - 1; ++klass) {
//                         int length = cvSliceLength(CV_WHOLE_SEQ, trees[klass].get_weak_predictors());
//                         CvMat *weakResponses = cvCreateMat(length, 1, CV_32FC1);
//                         klass5 = trees[klass].predict(query, NULL, weakResponses, CV_WHOLE_SEQ);
//                         
//                         double score = cvSum(weakResponses).val[0];
//                         scores[klass] = score;
//                         
//                         cvReleaseMat(&weakResponses);
//                     }
//                 }
//                 
//                 cout << "I think it's a ";
//                 if (BAYES_ON) cout << classIntToString(klass) << "(bayes) ";
//                 if (SVM_ON)   cout << classIntToString(klass2) << "(svm) ";
//                 if (KNN_ON)   cout << classIntToString(klass3) << "(knn) ";
//                 if (RTREE_ON) cout << classIntToString(klass4) << "(rtree) ";
//                 if (ALL_TREES_ON) { 
//                     cout << "\ntrees: ";
//                     for (int klass = 0; klass < kNumObjectTypes - 1; ++klass) {
//                         cout << classIntToString(klass) << " (" << scores[klass] << ") ";
//                     }
//                     cout << endl;
//                 }
//                 cout << endl;
//                 
//                 double max_score = 0;
//                 int max_klass = -1;
//                 for (int klass = 0; klass < kNumObjectTypes - 1; ++klass) {
//                     if (scores[klass] > max_score) {
//                         max_klass = klass;
//                         max_score = scores[klass];
//                     }
//                 }    
//         
//                 CObject o;
//                 o.rect = cvRect(x, y, 32*scale, 32*scale);
//                 o.label = classIntToString(max_klass);
//         
//                 showRect(dst, &o, &keypts);
//             }
//         }
//         
//         scale *= 1.1;
//     }
//     return true;
// }

bool FileExists(string strFilename) {
  struct stat stFileInfo;

  // Attempt to get the file attributes
  return stat(strFilename.c_str(),&stFileInfo) == 0;
  
}
 
// train
// Trains the classifier to recognize the objects given in the
// training file list.
bool Classifier::train()
{
    cout << "------------------------------------------" << endl;
    cout << "\t\tTraining" << endl;
    
    if (kmeans_load)
    kmeans( set_train.kmeans_input() );
    
    svm.train( &set_train );
    
    test();
}


bool Classifier::kmeans(DataSet *data) 
{
    cout << "------------------------------------------" << endl;
    cout << "\t\tK-Means" << endl;
    
    if (kmeans_load)
    {
        cout << "Loading..." << endl;
        
        centers = (CvMat *)cvLoad("centers.dat");
        data->samples = (CvMat *)cvLoad("samples.dat");
        data->responses = (CvMat *)cvLoad("responses.dat");
        data->centers = centers;
        
        cout << "Loaded Successfully" << endl;
        return;
    }
    
    CvMat *desc = data->kmeans_input();
    CvMat *clusters = cvCreateMat(data->num_samples, 1, CV_32SC1);
    centers = cvCreateMat(NUM_CLUSTERS, SURF_SIZE, CV_32FC1);

    cvKMeans2(
        all_desc,       // samples
        NUM_CLUSTERS,   // clusters
        clusters,       // labels
        cvTermCriteria(
            CV_TERMCRIT_EPS|CV_TERMCRIT_ITER, // End criteria
            10,                               // Max iter
            0.1),                             //accuracy
        1,              // attempts
        &rng,           //rng
        0,              // flags
        centers,        // centers
        NULL            // compactness
    );
    
    if (kmeans_save) 
    {
        cvSave("centers.dat", centers);
        cvSave("samples.dat", data->cluster_samples() );
        cvSave("responses.dat", data->cluster_responses() );
        cout << "Saved!" << endl;
    }
    
    cvReleaseMat(&desc);
    data->to_kmeans = NULL;
    
    return true;
}


// bool Classifier::train_rtree(CvMat *desc, CvMat *responses) 
// {
//     cout << "------------------------------------------" << endl;
//     cout << "Step Start: rtree" << endl; 
//     
//     CvRTParams params( 
//         5,                                 //Max depth 
//         2,                                 //Min sample count 
//         0,                                 //Regression accuracy 
//         false,                              //Use Surrogates 
//         10,                                 //Max Categories 
//         NULL,                               //Priors  
//         false,                              //Calculate Variables variance 
//         0,                                  //Nactive Vars 
//         500,                                //Max Tree Count 
//         0.1,                            //Accuracy to archive 
//         CV_TERMCRIT_EPS|CV_TERMCRIT_ITER    //Criteria to stop algorithm 
//     );
//     
//     // //printf("%d %d\n", NUM_CLUSTERS, desc->cols);
//     // assert(NUM_CLUSTERS == desc->cols);
//     // CvScalar avg, std;
//     // cvAvgSdv(desc, &avg, &std);
//     
//     // printf("variance %f mean %f\n", std.val[0], avg.val[0]);
//     
//     CvMat *var_type = cvCreateMat(desc->cols + 1, 1, CV_8U);
//     cvSet(var_type, cvScalarAll(CV_VAR_NUMERICAL));
//     cvSet2D(var_type, desc->cols, 0, cvScalar(CV_VAR_CATEGORICAL));
//     
//     if (RTREE_ON) rtree.train(
//         desc, 
//         CV_ROW_SAMPLE,
//         responses,
//         0,
//         0,
//         var_type,
//         0,
//         params
//     );
//     else cout << "RTREE is turned off" << endl;
//     
//     // const CvMat *imp = rtree.get_var_importance();
//     // if (!imp) cout << "can't get var importance" << endl;
//     // else {
//     
//     // cout << "Var importance: " << endl;
//     // for (int i = 0; i < imp->cols; i++)
//         // printf("%f ", cvGetReal2D(imp, 0, i));
//     // }
//     // cout << endl;
//     
//     cout << "Step Done:  rtree" << endl;
//     return true;
// }
// 
// bool Classifier::train_alltrees(CvMat *desc, CvMat *responses) 
// {
//     cout << "------------------------------------------" << endl;
//     cout << "Step Start: trees" << endl; 
//     
//     if (!ALL_TREES_ON) return false;
//     
//     CvMat* var_type = cvCreateMat( desc->cols + 1, 1, CV_8U );
//     for (int j = 0; j < desc->cols; ++j)
//         CV_MAT_ELEM(*var_type, unsigned char, j, 0) = CV_VAR_NUMERICAL;
//     CV_MAT_ELEM(*var_type, unsigned char, desc->cols, 0) = CV_VAR_CATEGORICAL;    
//     
//     CvBoostParams params( 
//         CvBoost::REAL, 
//         100, 
//         0.95, 
//         2, 
//         false, 
//         NULL 
//     );
//     params.split_criteria = CvBoost::DEFAULT;    
//     
//     CvMat *new_responses = cvCreateMat(responses->rows, 1, CV_32SC1);
//     for (int klass = 0; klass < kNumObjectTypes - 1; ++klass) {
//         if(klass == kOther) continue;
//         
//         cout << "Training: " << classIntToString(klass) << endl;
//         
//         for (int i = 0; i < responses->rows; ++i) {
//             int this_klass = CV_MAT_ELEM(*responses, int, i, 0);
//             int new_klass = this_klass == klass ? klass : kOther;
//             
//             CV_MAT_ELEM(*new_responses, int, i, 0) = new_klass;
//         }
//         
//         trees[klass].train(
//             desc,               // data
//             CV_ROW_SAMPLE,      // type
//             new_responses,      // responses
//             0,                  //
//             0,                  //
//             var_type,           // var_types
//             0,                  // 
//             params              // params
//         );
//     }
//     
//     //CvReleaseMat(&var_type);
//     //CvReleaseMat(&new_responses);
//     
//     cout << "Step Done:  mugtree" << endl;
//     return true;
// }

int best_cluster(CvMat *centers, float *vars)
{
    float min_distance = INT_MAX;
    int min_index = -1;
    for (int c = 0; c < NUM_CLUSTERS; ++c) {
        float dist = 0;
        for (int j = 0; j < SURF_SIZE; ++j) {
            dist += pow(vars[j] - cvGetReal2D(centers, c, j), 2);
        }
        
        if (dist < min_distance) {
            min_index = c;
            min_distance = dist;
        }
    }
    return min_index;
}

bool Classifier::test(DataSet *data)
{
    cout << "------------------------------------------" << endl;
    cout << "\t\tTesting" << endl;
    
    svm.scores.reset();

    for (int img = 0; img < data->num_images; ++img) 
    {
        showProgress(img, data->num_images);
        
        vector<int> cluster( data->count(img) );
        for (int i = 0; i < data->count(img); ++i)
            cluster[i] = best_cluster(centers, data->get(img, i) );
        
        
        CvMat *query = cvCreateMat(1, NUM_CLUSTERS, CV_32FC1);
        cvSet(query, cvScalar(0));
        for (int row = 0; row < data->count(img); ++row) {
            int cluster_idx = cluster[row];
            int oldVal = cvGetReal2D(query, 0, cluster_idx);
            cvSetReal2D(query, 0, cluster_idx, oldVal+1);//cvScalar(oldVal+1));
        }
        
        int trueClass = data->get_class(img);
        
        svm.test(query, trueClass);
        
        
        // if (ALL_TREES_ON) {
        // 
        //     for (int klass = 0; klass < kNumObjectTypes - 1; ++klass) {
        //         int length = cvSliceLength(CV_WHOLE_SEQ, trees[klass].get_weak_predictors());
        //         CvMat *weakResponses = cvCreateMat(length, 1, CV_32FC1);
        //         klass5 = trees[klass].predict(query, NULL, weakResponses, CV_WHOLE_SEQ);
        //         
        //         double score = cvSum(weakResponses).val[0];
        //         scores[klass] = score;
        //         
        //         cvReleaseMat(&weakResponses);
        //     }
        //                  
        //     double max_score = 0;
        //     int max_klass = -1;
        //     for (int klass = 0; klass < kNumObjectTypes - 1; ++klass) {
        //         if (scores[klass] > max_score) {
        //             max_klass = klass;
        //             max_score = scores[klass];
        //         }
        //     }
        // }
        
    }
    
    svm.score.out();
    
    return true;
}

bool Classifier::extract(TTrainingFileList& fileList, const char *featuresFile)
{
    cout << "Classes:" << endl;
    for (int i = 0; i < (int)fileList.classes.size(); i++) {
        cout << fileList.classes[i] << " (";
        int count = 0;
        for (int j = 0; j < (int)fileList.files.size(); j++) {
            if (fileList.files[j].label == fileList.classes[i]) {
                count += 1;
            }
        }
        cout << count << " samples)" << endl;
    }
    cout << endl;

    IplImage *image;
    
    int maxImages = INT_MAX;

    cout << "Processing images..." << endl;

    //vector<int> numIpoints(kNumObjectTypes);
    int minfiles = min(maxImages, (int)fileList.files.size());
    for (int i = 0; i < minfiles; i++) 
    {
        // skip too many others...
        if (fileList.files[i].label == "other" && --max_other < 0) continue;

        // show progress
        if (i % 10 == 0) showProgress(i, minfiles);

        image = cvLoadImage(fileList.files[i].filename.c_str(), 0);
        
        if (image == NULL) {
            cerr << "ERROR: could not load image " << fileList.files[i].filename.c_str() << endl;
            continue;
        }
        
        string label = fileList.files[i].label;

        CvSeq *keypoints = 0, *descriptors = 0;
        CvSURFParams params = cvSURFParams(100, SURF_SIZE == 128);
        cvExtractSURF(image, 0, &keypoints, &descriptors, storage, params);
        
        if (descriptors->total == 0) continue;
        
        vector<float> desc;
        desc.resize(descriptors->total * descriptors->elem_size/sizeof(float));
        cvCvtSeqToArray(descriptors, &desc[0]);
        
        vector<float *> features;
        int where = 0;
        for (int pt = 0; pt < keypoints->total; ++pt) {
            float *f = new float[SURF_SIZE];
            for (int j = 0; j < SURF_SIZE; ++j) {
                f[j] = desc[where];
                ++where;
            }
            features.push_back(f);
        }
        
        if (i % test_to_train == 1 || !test_on) 
            set_train.push(stringToClassInt(label), features);
        else set_test.push(stringToClassInt(label), features);
    }
    
    
    cout << endl << "Extracted surf features. " << endl;

    cout << "Train Set" << endl;
    set_train.stats();
    
    cout << "Test Set" << endl;
    set_test.stats();
    
    return true;
}


