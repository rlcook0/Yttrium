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

#include "statmodel.h"
#include "homography.h"

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
    kmeans_save = false;
    
    test_on = true;
    test_to_train = 4;
    centers = NULL;
    
    save_all = load_all = false;
    
    prevFrame = NULL;
    
    totalXDiff = totalYDiff = 0;
    
}
    
// destructor
Classifier::~Classifier()
{
}
 
 

bool Classifier::loadState()
{
    centers = (CvMat *)cvLoad("centers.dat");

    svm.load();
    rtrees.load();
    btrees.load();
    bayes.load();
    
    return true;
}
 
 
// train
// Trains the classifier to recognize the objects given in the
// training file list.
bool Classifier::train()
{
    cout << "------------------------------------------" << endl;
    cout << "\t\tTraining" << endl;
    
    kmeans( &set_train );
    
    if (load_all) {
        svm.load();
        rtrees.load();
        btrees.load();
        bayes.load();
        
    } else {
        svm.train( &set_train );
        rtrees.train( &set_train );
        btrees.train( &set_train );
        bayes.train( &set_train );
    
    }
    test( &set_train);
    test( &set_test);
    
    return true;
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
        return true;
    }
    
    CvMat *desc = data->kmeans_input();
    data->clusters = cvCreateMat(data->num_samples, 1, CV_32SC1);
    centers = cvCreateMat(num_clusters, SURF_SIZE, CV_32FC1);
    data->centers = centers;
    
    cout << "Running with k = " << num_clusters << endl;
    flush(cout);
    
    cvKMeans2(
        desc,           // samples
        num_clusters,   // clusters
        data->clusters, // labels
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

int Classifier::best_cluster(CvMat *centers, float *vars)
{
    float min_distance = INT_MAX;
    int min_index = -1;
    for (int c = 0; c < num_clusters; ++c) {
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
    
    for (int img = 0; img < data->num_images; ++img) 
    {
        showProgress(img, data->num_images);
        
        vector<int> cluster( data->count(img) );
        for (int i = 0; i < data->count(img); ++i)
            cluster[i] = best_cluster(centers, data->get(img, i) );
        
        
        CvMat *query = cvCreateMat(1, num_clusters, CV_32FC1);
        cvSet(query, cvScalar(0));
        for (int row = 0; row < data->count(img); ++row) {
            int cluster_idx = cluster[row];
            int oldVal = cvGetReal2D(query, 0, cluster_idx);
            cvSetReal2D(query, 0, cluster_idx, oldVal+1);//cvScalar(oldVal+1));
        }
        
        int trueClass = data->get_class(img);
        
        svm.test(query, trueClass);
        rtrees.test(query, trueClass);
        btrees.test(query, trueClass);
        bayes.test(query, trueClass);

    }
    
    svm.scores.out("SVM: ");
    rtrees.scores.out("RTrees: ");
    btrees.scores.out("BTrees: ");
    bayes.scores.out("Bayes: ");
    
    return true;
}

bool Classifier::extract(TTrainingFileList& fileList)
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
        if (fileList.files[i].label == "other" && --max_others < 0) continue;

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
    set_train.recount();
    set_train.stats();
    
    cout << "Test Set" << endl;
    set_train.recount();
    set_test.stats();
    
    return true;
}



/*----------------------------------------------------------------------------------------------------------
#############################################################################################################*/


CvMat *normalize(const CvMat* vector) {
    CvMat *norm = cvCloneMat(vector);
    double normVal = cvNorm(vector);
    cvScale(vector, norm, 1 / normVal);
    
    return norm;
}

// TESTING ONLY
bool Classifier::showRect(IplImage *image, CObject *rect, const vector<CvSURFPoint> *pts) {
    //char *WINDOW_NAME = "test";
    CvFont font;

    IplImage *frameCopy = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
    cvConvertImage(image, frameCopy);
        
    cvNamedWindow("test", CV_WINDOW_AUTOSIZE);
    cvInitFont(&font, CV_FONT_VECTOR0, 0.75, 0.75, 0.0f, 1, CV_AA);
    
    if (pts != NULL) {
        CvScalar color = CV_RGB(255, 255, 255);
        for (int i = 0; i < (int)pts->size(); ++i) {
            //printf("(%f %f)->(%d %d) ", (*pts)[i].x, (*pts)[i].y, frameCopy->width, frameCopy->height);
            cvRectangle(frameCopy, cvPoint((*pts)[i].pt.x, (*pts)[i].pt.y), 
                cvPoint((*pts)[i].pt.x + 3, (*pts)[i].pt.y + 3), color);
        }
    }
    
    rect->draw(frameCopy, CV_RGB(255, 0, 255), &font);
        
    cvShowImage("test", frameCopy);
    cvReleaseImage(&frameCopy);

    return cvWaitKey(10) != -1;
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
    double xDiff = 0, yDiff = 0;
    optical_flow(frame, &xDiff, &yDiff);
    
    totalXDiff += xDiff;
    totalYDiff += yDiff;
    if (!scored)
        return true;
    
    cout << "--------------------------------------" << endl;
    cout << "\t\tRun" << endl;
    
    assert((frame != NULL) && (objects != NULL));
    
    printf("Let's go!\n");
    
    for (int i = 0; i < (int)prevObjects.size(); ++i) {
        if (prevObjects[i].rect.x > -20 && prevObjects[i].rect.x < frame->width 
         && prevObjects[i].rect.y > -20 && prevObjects[i].rect.y < frame->height)
            objects->push_back(prevObjects[i]);
    }
    
    //printf("HEY OPTICAL FLOW!!!! %f %f\n", totalXDiff, totalYDiff);
    
    // move old objects
    for (int i = 0; i < (int)objects->size(); ++i) {
        (*objects)[i].rect.x -= totalXDiff * 10;
        (*objects)[i].rect.y -= totalYDiff * 10;
    }
    
    cout << "Flow: " << totalXDiff << " " << totalYDiff << endl;
    totalYDiff = 0;
    totalXDiff = 0;
    
    
    // Convert to grayscale.
    IplImage *gray  = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
    cvCvtColor(frame, gray, CV_BGR2GRAY);
    
    // Resize by half first, as per the handout.
    double scale = 2.0;
    IplImage *dst = cvCreateImage(cvSize(gray->width  / scale, gray->height  / scale), gray->depth,  gray->nChannels);
    cvResize(gray, dst);

    printf("About to do SURF\n");
    CvSeq *keypoints = 0, *descriptors = 0;
    CvSURFParams params = cvSURFParams(100, SURF_SIZE == 128);
    cvExtractSURF(dst, 0, &keypoints, &descriptors, storage, params);
    
    cout << "desc: " << descriptors->total << endl;
    if (descriptors->total == 0) return false;
    
    vector<float> desc;
    desc.resize(descriptors->total * descriptors->elem_size/sizeof(float));
    cvCvtSeqToArray(descriptors, &desc[0]);
    
    vector<CvSURFPoint> keypts;
    keypts.resize(keypoints->total);
    cvCvtSeqToArray(keypoints, &keypts[0]);
    
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
    printf("Done SURF\n");

    printf("Clustering...\n");
    vector<int> cluster(features.size());
    for (int i = 0; i < (int)features.size(); ++i) {
        cluster[i] = best_cluster(centers, features[i]);
    }
    printf("Done clustering...\n");
    
    vector<FoundObject> newObjects;
    run_boxscan(dst, cluster, keypts, features, newObjects, objects);
    for (int i = 0; i < (int)newObjects.size(); ++i) {
        if (newObjects[i].object.rect.x > -20 && newObjects[i].object.rect.x < frame->width 
         && newObjects[i].object.rect.y > -20 && newObjects[i].object.rect.y < frame->height)
            objects->push_back(newObjects[i].object);
    }
    
    prevObjects = *objects;
    
    cvReleaseImage(&gray);
  
    return true;
}

void Classifier::optical_flow(const IplImage *frame, double *xD, double *yD) {
    double xDiff = 0;
  double yDiff = 0;
  //double xQDiff = 0;
  //double yQDiff = 0;
  if (prevFrame) {
	  /* Optical flow for entire image */
	  CvSize img_sz = cvGetSize(frame);
	  
	  IplImage *imgA = cvCreateImage(img_sz, IPL_DEPTH_8U, 1);
	  IplImage *imgB = cvCreateImage(img_sz, IPL_DEPTH_8U, 1);
	  
	  cvCvtColor(frame, imgA, CV_BGR2GRAY);
	  cvCvtColor(prevFrame, imgB, CV_BGR2GRAY);
	  
	  CvMat* velx = cvCreateMatHeader( img_sz.height, img_sz.width, CV_32FC1 );   
      cvCreateData( velx );   
      CvMat* vely = cvCreateMatHeader( img_sz.height, img_sz.width, CV_32FC1 );   
      cvCreateData( vely );
	  
	  cvCalcOpticalFlowLK(
		imgA,
		imgB,
		cvSize(15, 15),
		velx,
		vely
	  );
	  
	  xDiff = cvAvg(velx).val[0];
	  yDiff = cvAvg(vely).val[0];
	  
      *xD = xDiff;
      *yD = yDiff;

  } // if
  else {
	prevFrame = cvCreateImage (
		cvGetSize(frame),
		frame->depth,
		frame->nChannels
	);
  } // else  
  
	cvCopy(frame, prevFrame);	
}

bool Classifier::run_boxscan(IplImage *dst, vector<int> &cluster, vector<CvSURFPoint> &keypts, vector<float *> &pts, vector<FoundObject> &newObjects, const CObjectList *oldObjects)
{
    float scale = 2.0f;
    int maxWidth = dst->width;
    int maxHeight = dst->height;

    int numLayers = 7;
    while(numLayers-- > 0) {
        for (int x = 0; x < maxWidth - 32*scale; x += 8) {
            for (int y = 0; y < maxHeight - 32*scale; y += 8) {
                vector<int> newpts;
                
                for (int i = 0; i < (int)pts.size(); ++i) {
                    if (keypts[i].pt.x >= x && keypts[i].pt.x < x + 32*scale 
                        && keypts[i].pt.y >= y && keypts[i].pt.y < y + 32*scale)
                        newpts.push_back(i);
                }
                
                if (newpts.size() < MIN_IPOINTS) continue;

                CvMat *query = cvCreateMat(1, num_clusters, CV_32FC1);
                cvSet(query, cvScalar(0));
                for (int row = 0; row < (int)newpts.size(); ++row) {
                    int idx = newpts[row];
                    int cluster_idx = cluster[idx];
                    
                    int oldVal = cvGetReal2D(query, 0, cluster_idx);
                    cvSetReal2D(query, 0, cluster_idx, oldVal+1);
                    
                    //cout << row << " " << idx << " " << cluster_idx << " " << oldVal << endl;
                }
        
                float scores[kNumObjectTypes];
                int klass = btrees.predict(query, scores);
                int min_score = 0;
                                
//                cout << "I think it's a ";
                for (int klass = 0; klass < kNumObjectTypes - 1; ++klass) {
//                    cout << classIntToString(klass) << " (" << scores[klass] << ") ";
                    min_score = (scores[klass] < min_score) ? scores[klass] : min_score;
                }
//                cout << endl;
                
                
                if (klass != kOther) {
                    FoundObject fo;
                    CObject o;
                    o.rect = cvRect(x * scale, y * scale, 48*scale, 48*scale);
                    o.label = classIntToString(klass);
                    fo.object = o;
                    fo.score = min_score * scale;
                    
                    newObjects.push_back(fo);
                }
                
                             //    
                             // CObject o;
                             // o.rect = cvRect(x, y, 32*scale, 32*scale);
                             // o.label = classIntToString(klass);
        
               // showRect(dst, &o, &keypts);
            }
        }
        
        scale *= 1.1;
    }
    // Possibly add found objects
    vector<int> toRemove;

    float minScore = 100.0f;
    FoundObject minScoreObject;
    // prefer lower-scoring
    for (int i = 0; i < (int)newObjects.size(); ++i) {
        if (newObjects[i].score < minScore) {
            minScore = newObjects[i].score;
            minScoreObject = newObjects[i];
        }
    }
    // find larger objects
    bool foundbigger = true;
    while(foundbigger) {
        foundbigger = false;
        for (int i = (int)newObjects.size() - 1; i >= 0; --i) {
            if (newObjects[i].object.overlap(minScoreObject.object) > minScoreObject.object.area() * 0.8
                && newObjects[i].object.area() > minScoreObject.object.area()) {
                foundbigger = true;
                minScoreObject = newObjects[i];
            }
        }
    }

    newObjects.clear();
    newObjects.push_back(minScoreObject);
    // for (int i = 0; i < (int)toRemove.size(); ++i) {
        // newObjects.erase(newObjects.begin() + toRemove[i]);
    // }
    // toRemove.clear();

    // dont overlap with old objects
    toRemove.clear();
    for (int i = 0; i < (int)newObjects.size(); ++i) {
        const CObject &o = newObjects[i].object;
        for (int j = 0; j < (int)(*oldObjects).size(); ++j) {
            int overlap = o.overlap((*oldObjects)[j]);
            if (overlap > 1) {
                toRemove.push_back(i);
                break;
            }
        }
    }
    for (int i = 0; i < (int)toRemove.size(); ++i) {
        newObjects.erase(newObjects.begin() + toRemove[i]);
    }
    toRemove.clear();

    return true;
}

