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

#include "homography.h"

#define BAYES_ON false
#define SVM_ON false
#define KNN_ON false
#define RTREE_ON false
#define ALL_TREES_ON true

#define LOAD_KMEANS false
#define SAVE_KMEANS true
#define LOAD_IPOINTS false
#define SAVE_IPOINTS true

// Classifier class ---------------------------------------------------------
 
// default constructor
Classifier::Classifier()
{
    // initalize the random number generator (for sample code)
    rng = cvRNG(-1);
    //srand(time(NULL));
 
    storage = cvCreateMemStorage(0);
    prevFrame = NULL;
    
    totalXDiff = 0;
    totalYDiff = 0;
 
    //centers = NULL;
 
    //_features.load("dict.xml");
    //_regressor = new LogReg(_features.numFeatures(), double(0.001));
    
}
    
// destructor
Classifier::~Classifier()
{
    //delete _regressor;
    // CS221 TO DO: free any memory allocated by the object
}
 
// loadState
// Configure the classifier from the given file.
bool Classifier::loadState(const char *filename)
{
    assert(filename != NULL);
    

    if (BAYES_ON) bayes.load("bayes.dat");
    if (SVM_ON)   svm.load("svm.dat");
    
    if (KNN_ON)   
    {
        CvMat *desc = (CvMat *)cvLoad("desc.dat");
        CvMat *responses = (CvMat *)cvLoad("responses.dat");
        knn.train(desc, responses);
    }
    if (RTREE_ON) rtree.load("rtree.dat");
    if (ALL_TREES_ON) {
        for (int i = 0; i < kNumObjectTypes - 1; ++i) {
            stringstream filename;
            filename << "trees" << i << ".dat";
            trees[i].load(filename.str().c_str());
        }
    }
    centers = (CvMat *)cvLoad("centers.dat");
        
    assert(centers != NULL);    
    
    //loadSURFFile(filename);
    //setupKDTree();
    
    return true;
}
 
// saveState
// Writes classifier configuration to the given file
bool Classifier::saveState(const char *filename)
{
    assert(filename != NULL);
    
    //saveSURFFile(filename);
    
    return true;
}
 
 
// saveState
// Writes classifier TrainingFile to the given file
bool Classifier::saveSURFFile(const char *filename)
{
    if (filename == NULL) return false;
    
    ofstream outfile;
    outfile.open(filename);
    
    outfile << surfFeatures.size() << endl;
    
    for(map<string, vector<Ipoint> >::iterator it = surfFeatures.begin(); it != surfFeatures.end(); ++it)
    {
        outfile << it->second.size() << " " << it->first << endl; 
        for (unsigned j = 0; j < it->second.size(); j++)
        {
            Ipoint i = (it->second)[j];
            outfile << i.x << " " << i.y << " " << i.scale << " " << i.orientation << " " << i.laplacian << " ";
            for (unsigned k = 0; k < SURF_SIZE; k++)
                outfile << i.descriptor[k] << " ";
            outfile << endl;
        }
        outfile << endl;
    }
    outfile.close();
    
    cout << "Wrote SURF descriptor values to: " << filename << endl;

    return true;
}


bool Classifier::loadSURFFile(const char *filename)
{
    if (filename == NULL) return false;
    
    ifstream infile;
    infile.open(filename);
    
    int count, ipoints;
    
    infile >> count; infile.ignore(1); //\n
    
    for (int c = 0; c < count; c++) 
    {
	    
        string name;
        infile >> ipoints; infile.ignore(1);        
        infile >> name;    infile.ignore(1);
        
        printf("reading %s: %d\n", name.c_str(), ipoints);
        
        vector<Ipoint> v;
        for (int j = 0; j < ipoints; j++) 
        {
            Ipoint i;
            infile >> i.x >> i.y >> i.scale >> i.orientation >> i.laplacian;
            for (int k = 0; k < SURF_SIZE; k++)
                infile >> i.descriptor[k];
            
            v.push_back(i);   
            infile.ignore(1); //\n
        }
        surfFeatures[name] = v;
        infile.ignore(1);  // \n
    }
    infile.close();
    
    cout << "Loaded SURF descriptor values from: " << filename << endl;
    
    
    int total = 0;
    for (map<string, vector<Ipoint> >::const_iterator it = surfFeatures.begin(); it != surfFeatures.end(); ++it) {
        total += it->second.size();
        printf("%s %d\n", it->first.c_str(), total);
        surfThresh.push_back(pair<string, int>(it->first, total));
        surfTotal[it->first] = (int)(it->second.size());
    }
    surfTotalIpoints = total;
    printf("how many? %d\n", total);
    
    return true;
}


bool Classifier::setupKDTree()
{
    CvMat *desc = cvCreateMat(surfTotalIpoints, SURF_SIZE, CV_32FC1);

    int where = 0;
    for (map<string, vector<Ipoint> >::const_iterator it = surfFeatures.begin(); it != surfFeatures.end(); ++it) {
        const vector<Ipoint> *pts = &(it->second);
        for (unsigned i = 0; i < it->second.size(); ++i) {
            const Ipoint *ipt = &((*pts)[i]);
            const float *vals = ipt->descriptor;
            for (int j = 0; j < SURF_SIZE; ++j) {
                //printf(" (%d %d %f)", where, j, vals[j]);
                cvSetReal2D(desc, where, j, vals[j]);
            }
            ++where;
        }
    }
    surfFT = cvCreateKDTree(desc);
    printf("made surf tree!\n");
    
    return true;
}

int Classifier::stringToClassInt(string type) 
{
    if (type == "other")    return kOther;
    if (type == "mug")      return kMug;
    if (type == "keyboard") return kKeyboard;
    if (type == "clock")    return kClock;
    if (type == "stapler")  return kStapler;
    if (type == "scissors") return kScissors;
    
    return -1;
}

string Classifier::classIntToString(int type) 
{
    if (type == kOther)     return "other";
    if (type == kMug)       return "mug";
    if (type == kKeyboard)  return "keyboard";
    if (type == kClock)     return "clock";
    if (type == kStapler)   return "stapler";
    if (type == kScissors)  return "scissors";
    
    return "error";
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
    
    printf("HEY OPTICAL FLOW!!!! %f %f\n", totalXDiff, totalYDiff);
    
    // move old objects
    for (int i = 0; i < (int)objects->size(); ++i) {
        (*objects)[i].rect.x -= totalXDiff;
        (*objects)[i].rect.y -= totalYDiff;
    }
    totalXDiff = 0;
    totalYDiff = 0;
    
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
    
    vector<feat> features;
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
        float min_distance = INT_MAX;
        int min_index = -1;
        for (int c = 0; c < NUM_CLUSTERS; ++c) {
            float dist = 0;
            for (int j = 0; j < SURF_SIZE; ++j) {
                dist += pow(features[i][j] - cvGetReal2D(centers, c, j), 2);
            }

            if (dist < min_distance) {
                min_index = c;
                min_distance = dist;
            }
        }
        
        cout << min_index << endl;
        cluster[i] = min_index;
    }
    printf("Done clustering...\n");
    
    vector<FoundObject> newObjects;
    run_boxscan(dst, cluster, keypts, features, newObjects, objects);
    for (int i = 0; i < (int)newObjects.size(); ++i) {
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
	   // CvPoint p0 = cvPoint(
		 // img_sz.width/2,
		 // img_sz.height/2
	   // );
	   // CvPoint p1 = cvPoint(
		 // img_sz.width/2 - xDiff*100,
		 // img_sz.height/2 - yDiff*100
	   // );
	   
	   //cvLine( imgA, p0, p1, CV_RGB(255,0,0),2 );
	   
	  /* Optical flow for first quadrant */
	  // CvSize quadrant_size = cvSize(img_sz.width/2, img_sz.height/2);
	  
	  // IplImage *imgQA = cvCreateImage(img_sz, IPL_DEPTH_8U, 1);
	  // IplImage *imgQB = cvCreateImage(img_sz, IPL_DEPTH_8U, 1);
	  
	  // cvCvtColor(frame, imgQA, CV_BGR2GRAY);
	  // cvCvtColor(prevFrame, imgQB, CV_BGR2GRAY);
	  
	  // cvSetImageROI(imgQA, cvRect(0,0,quadrant_size.width,quadrant_size.height));
	  // cvSetImageROI(imgQB, cvRect(0,0,quadrant_size.width,quadrant_size.height));
	  
	  // CvMat* velQx = cvCreateMatHeader( quadrant_size.height, quadrant_size.width, CV_32FC1 );   
      // cvCreateData( velQx );   
      // CvMat* velQy = cvCreateMatHeader( quadrant_size.height, quadrant_size.width, CV_32FC1 );   
      // cvCreateData( velQy );
	  
	  // cvCalcOpticalFlowLK(
		// imgQA,
		// imgQB,
		// cvSize(15, 15),
		// velQx,
		// velQy
	  // );
	  
	  // xQDiff = cvAvg(velQx).val[0];
	  // yQDiff = cvAvg(velQy).val[0];
	  
	   // CvPoint pQ0 = cvPoint(
		 // quadrant_size.width/2,
		 // quadrant_size.height/2
	   // );
	   // CvPoint pQ1 = cvPoint(
		 // quadrant_size.width/2 - xQDiff*50,
		 // quadrant_size.height/2 - yQDiff*50
	   // );
	   
	   // cvLine( imgA, pQ0, pQ1, CV_RGB(255,0,0),2 );
	   
	  // /* Optical flow for second quadrant */
	  // cvSetImageROI(imgQA, cvRect(quadrant_size.width,0,quadrant_size.width,quadrant_size.height));
	  // cvSetImageROI(imgQB, cvRect(quadrant_size.width,0,quadrant_size.width,quadrant_size.height));
	  
	  // velQx = cvCreateMatHeader( quadrant_size.height, quadrant_size.width, CV_32FC1 );   
      // cvCreateData( velQx );   
      // velQy = cvCreateMatHeader( quadrant_size.height, quadrant_size.width, CV_32FC1 );   
      // cvCreateData( velQy );
	  
	  // cvCalcOpticalFlowLK(
		// imgQA,
		// imgQB,
		// cvSize(15, 15),
		// velQx,
		// velQy
	  // );
	  
	  // xQDiff = cvAvg(velQx).val[0];
	  // yQDiff = cvAvg(velQy).val[0];
	  
	   // pQ0 = cvPoint(
		 // 3*quadrant_size.width/2,
		 // quadrant_size.height/2
	   // );
	   // pQ1 = cvPoint(
		 // 3*quadrant_size.width/2 - xQDiff*50,
		 // quadrant_size.height/2 - yQDiff*50
	   // );
	   
	   // cvLine( imgA, pQ0, pQ1, CV_RGB(255,0,0),2 );
			
	   // /* Optical flow for third quadrant */	  
	  // cvSetImageROI(imgQA, cvRect(0,quadrant_size.height,quadrant_size.width,quadrant_size.height));
	  // cvSetImageROI(imgQB, cvRect(0,quadrant_size.height,quadrant_size.width,quadrant_size.height));
	  
	  // velQx = cvCreateMatHeader( quadrant_size.height, quadrant_size.width, CV_32FC1 );   
      // cvCreateData( velQx );   
      // velQy = cvCreateMatHeader( quadrant_size.height, quadrant_size.width, CV_32FC1 );   
      // cvCreateData( velQy );
	  
	  // cvCalcOpticalFlowLK(
		// imgQA,
		// imgQB,
		// cvSize(15, 15),
		// velQx,
		// velQy
	  // );
	  
	  // xQDiff = cvAvg(velQx).val[0];
	  // yQDiff = cvAvg(velQy).val[0];
	  
	   // pQ0 = cvPoint(
		 // quadrant_size.width/2,
		 // 3*quadrant_size.height/2
	   // );
	   // pQ1 = cvPoint(
		 // quadrant_size.width/2 - xQDiff*50,
		 // 3*quadrant_size.height/2 - yQDiff*50
	   // );
	   
	   // cvLine( imgA, pQ0, pQ1, CV_RGB(255,0,0),2 );
	   
	   // /* Optical flow for fourth quadrant */	  	  
	  // cvSetImageROI(imgQA, cvRect(quadrant_size.width,quadrant_size.height,quadrant_size.width,quadrant_size.height));
	  // cvSetImageROI(imgQB, cvRect(quadrant_size.width,quadrant_size.height,quadrant_size.width,quadrant_size.height));
	  
	  // velQx = cvCreateMatHeader( quadrant_size.height, quadrant_size.width, CV_32FC1 );   
      // cvCreateData( velQx );   
      // velQy = cvCreateMatHeader( quadrant_size.height, quadrant_size.width, CV_32FC1 );   
      // cvCreateData( velQy );
	  
	  // cvCalcOpticalFlowLK(
		// imgQA,
		// imgQB,
		// cvSize(15, 15),
		// velQx,
		// velQy
	  // );
	  
	  // xQDiff = cvAvg(velQx).val[0];
	  // yQDiff = cvAvg(velQy).val[0];
	  
	   // pQ0 = cvPoint(
		 // 3*quadrant_size.width/2,
		 // 3*quadrant_size.height/2
	   // );
	   // pQ1 = cvPoint(
		 // 3*quadrant_size.width/2 - xQDiff*50,
		 // 3*quadrant_size.height/2 - yQDiff*50
	   // );
	   
	   // cvLine( imgA, pQ0, pQ1, CV_RGB(255,0,0),2 );
	   
	  // cvNamedWindow("Optical FLOW",0);
	  // cvShowImage("Optical FLOW",imgA);
	  
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

bool Classifier::run_boxscan(IplImage *dst, vector<int> &cluster, vector<CvSURFPoint> &keypts, vector<feat> &pts, vector<FoundObject> &newObjects, const CObjectList *oldObjects)
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

                CvMat *query = cvCreateMat(1, NUM_CLUSTERS, CV_32FC1);
                cvSet(query, cvScalar(0));
                for (int row = 0; row < (int)newpts.size(); ++row) {
                    int idx = newpts[row];
                    int cluster_idx = cluster[idx];
                    
                    int oldVal = cvGetReal2D(query, 0, cluster_idx);
                    cvSetReal2D(query, 0, cluster_idx, oldVal+1);
                    
                    //cout << row << " " << idx << " " << cluster_idx << " " << oldVal << endl;
                }
        
                int klass, klass2, klass3, klass4, klass5;
                double scores[kNumObjectTypes - 1];
                
                if (BAYES_ON) klass = bayes.predict(query);
                if (SVM_ON)   klass2 = svm.predict(query);
                if (KNN_ON)   klass3 = knn.find_nearest(query, 5);
                if (RTREE_ON) klass4 = rtree.predict(query);
                if (ALL_TREES_ON) {
                    //cout << "avg: " << cvAvg(query).val[0] << endl;
                    
                    for (int klass = 0; klass < kNumObjectTypes - 1; ++klass) {
                        int length = cvSliceLength(CV_WHOLE_SEQ, trees[klass].get_weak_predictors());
                        CvMat *weakResponses = cvCreateMat(length, 1, CV_32FC1);
                        klass5 = trees[klass].predict(query, NULL, weakResponses, CV_WHOLE_SEQ);
                        
                        scores[klass] = cvSum(weakResponses).val[0];
                        
                        //cout << klass << " " << klass5 << endl;
                    }
                    
                    double min_score = 0;
                    int max_klass = -1;
                    for (int klass = 0; klass < kNumObjectTypes - 1; ++klass) {
                        if (scores[klass] < min_score) {
                            max_klass = klass;
                            min_score = scores[klass];
                        }
                    }
                    
                    klass5 = min_score < -3 ? max_klass : kOther;
                    if (klass5 != kOther) {
                        FoundObject fo;
                        CObject o;
                        o.rect = cvRect(x * scale, y * scale, 48*scale, 48*scale);
                        o.label = classIntToString(klass5);
                        fo.object = o;
                        fo.score = min_score * scale;
                        
                        newObjects.push_back(fo);
                    }
                }
                
                cout << "I think it's a ";
                if (BAYES_ON) cout << classIntToString(klass) << "(bayes) ";
                if (SVM_ON)   cout << classIntToString(klass2) << "(svm) ";
                if (KNN_ON)   cout << classIntToString(klass3) << "(knn) ";
                if (RTREE_ON) cout << classIntToString(klass4) << "(rtree) ";
                if (ALL_TREES_ON) { 
                    cout << "\ntrees: ";
                    for (int klass = 0; klass < kNumObjectTypes - 1; ++klass) {
                        cout << classIntToString(klass) << " (" << scores[klass] << ") ";
                    }
                    cout << endl;
                }
                cout << endl;
        
                CObject o;
                o.rect = cvRect(x, y, 32*scale, 32*scale);
                o.label = classIntToString(klass5);
        
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
            if (overlap > o.rect.width * o.rect.height * 0.3) {
                toRemove.push_back(i);
                break;
            }
        }
    }
    for (int i = 0; i < (int)toRemove.size(); ++i) {
        newObjects.erase(newObjects.begin() + toRemove[i]);
    }
    toRemove.clear();
    
    cout << "SIZE------------" << newObjects.size() << endl;
    
    return true;
}

bool FileExists(string strFilename) {
  struct stat stFileInfo;
  bool blnReturn;
  int intStat;

  // Attempt to get the file attributes
  intStat = stat(strFilename.c_str(),&stFileInfo);
  if(intStat == 0) {
    blnReturn = true;
  } else {
    blnReturn = false;
  }
  
  return(blnReturn);
}
 
// train
// Trains the classifier to recognize the objects given in the
// training file list.
bool Classifier::train(TTrainingFileList& fileList, const char *trainingFile)
{
    cout << "------------------------------------------" << endl;
    cout << "\t\tTraining" << endl;

    if (!LOAD_IPOINTS || !FileExists("/tmp/surf/num_images")) {
        extract(fileList, trainingFile);
    // } else {
        // int numImages;
        
        // ifstream num;
        // num.open("/tmp/surf/num_images");
        // num >> numImages;
        // num.close();
        
        // printf("Loading %d saved images...\n", numImages);
        
        // surfTotalIpoints = 0;
        // for (int i = 0; i < numImages; ++i) {
            // showProgress(i, numImages);
            // char klassfn[50], datfn[50];
            // sprintf(klassfn, "/tmp/surf/img%d.klass", i);
            // sprintf(datfn, "/tmp/surf/img%d.dat", i);
            
            // string klass;
            
            // ifstream klassf;
            // klassf.open(klassfn);
            // klassf >> klass;
            // klassf.close();
            
            // vector<Ipoint> pts;
            // loadSurf(datfn, pts);
            
            // surfTotalIpoints += (int)pts.size();
            
            // allImages.push_back(pair<string, vector<Ipoint> >(klass, pts));
        // }
        
        // printf("Done!\n");
    }
    
    // if (!LOAD_IPOINTS && SAVE_IPOINTS) {
        // printf("Saving %d images...\n", (int)allImages.size());
        // for (int i = 0; i < (int)allImages.size(); ++i) {
            // showProgress(i, (int)allImages.size());
            
            // char klassfn[50], datfn[50];
            // sprintf(klassfn, "/tmp/surf/img%d.klass", i);
            // sprintf(datfn, "/tmp/surf/img%d.dat", i);
            
            // saveSurf(datfn, allImages[i].second);
            
            // ofstream klass;
            // klass.open(klassfn);
            // klass << allImages[i].first;
            // klass.close();
        // }
        
        // ofstream num;
        // num.open("/tmp/surf/num_images");
        // num << (int)allImages.size();
        // num.close();
        // printf("Done!\n");
    // }
    
    // make folds
    cout << "making folds..." << endl;
    vector<int> indexes;
    for (int i = 0; i < (int)allImages.size(); ++i) indexes.push_back(i);
    random_shuffle ( indexes.begin(), indexes.end() );
    
    int size = (int)((double)allImages.size() * 0.8);
    trainSet = new class_feat_vec_star();
    testSet = new class_feat_vec_star();
    
    int trainTypes[kNumObjectTypes], testTypes[kNumObjectTypes];
    for(int i = 0; i < kNumObjectTypes; ++i)
        trainTypes[i] = testTypes[i] = 0;
    
    for (int i = 0; i < (int)allImages.size(); ++i) {
        int real = indexes[i];
        if (i < size) {
            trainTypes[stringToClassInt(allImages[real].first)] += 1;
            trainSet->push_back(&(allImages[real]));
        } else {
            testTypes[stringToClassInt(allImages[real].first)] += 1;
            testSet->push_back(&(allImages[real]));
        }
    }
    
    for(int i = 0; i < kNumObjectTypes; ++i) {
        printf("%s has %d in train and %d in test\n", classIntToString(i).c_str(), trainTypes[i], testTypes[i]);
    }
    printf("total: %d in train and %d in test\n", (int)trainSet->size(), (int)testSet->size());
    
    
    // testSet = trainSet = &allImages;
    
    CvMat *desc, *responses;
    if(!LOAD_KMEANS || !FileExists("desc.dat")) {
        desc = cvCreateMat(trainSet->size(), NUM_CLUSTERS, CV_32FC1);
        responses = cvCreateMat(trainSet->size(), 1, CV_32SC1);

        cout << "massaging responses..." << endl;
        for (int i = 0; i < (int)trainSet->size(); ++i) 
        {
            int klass = stringToClassInt((*trainSet)[i]->first);
            cvSetReal1D(responses, i, klass);
        }

        train_kmeans(desc);
    } else {
        printf("Loading kmeans...");
        desc = (CvMat *)cvLoad("desc.dat");
        centers = (CvMat *)cvLoad("centers.dat");
        responses = (CvMat *)cvLoad("responses.dat");
        printf(" Loaded!\n");
    }
    
    if (!LOAD_KMEANS && SAVE_KMEANS) {
        printf("Saving desc, kmeans, and responses...");
        cvSave("desc.dat", desc);
        cvSave("responses.dat", responses);
        cvSave("centers.dat", centers);
        printf(" Saved!\n");
    }

    train_bayes(desc, responses);
    train_svm(desc, responses);
    train_knn(desc, responses);
    train_rtree(desc, responses);
    train_alltrees(desc, responses);
    
    printf("Saving classifiers...");
    if (BAYES_ON) bayes.save("bayes.dat");
    if (SVM_ON) svm.save("svm.dat");
    //if (KNN_ON) knn.save("knn.dat");
    if (RTREE_ON) rtree.save("rtree.dat");
    if (ALL_TREES_ON) {
        for (int i = 0; i < kNumObjectTypes - 1; ++i) {
            stringstream filename;
            filename << "trees" << i << ".dat";
            trees[i].save(filename.str().c_str());
        }
    }
    printf(" Saved!\n");
    
    //train_test(trainSet, false);
    train_test(testSet, true);
    
    return true;
}


bool Classifier::train_kmeans(CvMat *desc) 
{
    cout << "------------------------------------------" << endl;
    cout << "Step Start: kmeans" << endl;
    
    cout << "setting first descriptors..." << endl;
    
    int trainIpoints = 0;
    for (int i = 0; i < (int)trainSet->size(); ++i) {
        trainIpoints += (*trainSet)[i]->second.size();
    }
    
    printf("We have %d ipoints\n", trainIpoints);
    
    CvMat *all_desc = cvCreateMat(trainIpoints, SURF_SIZE, CV_32FC1);
    int where = 0;
    for (int i = 0; i < (int)trainSet->size(); ++i) {
        for (int iptNo = 0; iptNo < (int)(*trainSet)[i]->second.size(); ++iptNo) {
            float *pt = (*trainSet)[i]->second[iptNo];
            for (int j = 0; j < SURF_SIZE; ++j) {
                cvSetReal2D(all_desc, where, j, pt[j]);
            }
            ++where;
        }
    }
    
    printf("We used %d of them\n", where);
    
    cout << "kmeans starting..." << endl;
    
    CvMat *clusters = cvCreateMat(trainIpoints, 1, CV_32SC1);
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
    
    cvReleaseMat(&all_desc);
    
    cout << "creating new descriptors..." << endl;
    
    cvSet(desc, cvRealScalar(0));
    where = 0;
    for(int i = 0; i < (int)(*trainSet).size(); ++i) {
        for (int iptNo = 0; iptNo < (int)(*trainSet)[i]->second.size(); ++iptNo) {
            int cluster = cvGetReal1D(clusters, where);
            
            //printf("(%d %d)\n", i, cluster);
            int oldVal = cvGetReal2D(desc, i, cluster);
            cvSetReal2D(desc, i, cluster, oldVal+1);
            ++where;
        }
        
        // double sum = 0;
        // for (int j = 0; j < NUM_CLUSTERS; ++j)
            // sum += cvGetReal2D(desc, i, j);
        
        // for (int j = 0; j < NUM_CLUSTERS; ++j) {
            // double val = cvGetReal2D(desc, i, j);
            // double newval = val / sum;
            // cvSetReal2D(desc, i, j, newval);
        // }    
    }
    CvScalar avg = cvAvg(desc);
    printf("we used %d ipoints in our matrix, for a mean of %f\n", where, avg.val[0]);
    
    cout << "Step Done:  kmeans" << endl; 
    return true;
}

bool Classifier::train_bayes(CvMat *desc, CvMat *responses) 
{
 
    cout << "------------------------------------------" << endl;
    cout << "Step Start: bayes" << endl; 
    
    
    
    if (BAYES_ON) bayes.train(desc, responses);
    else cout << "Bayes is turned off" << endl;
    
    return true;
}

bool Classifier::train_svm(CvMat *desc, CvMat *responses) 
{
    cout << "------------------------------------------" << endl;
    cout << "Step Start: svm" << endl; 
    
    if (SVM_ON) svm.train(desc, responses);
    else cout << "SVM is turned off" << endl;
    
    cout << "Step Done:  svm" << endl;
    return true;
}

bool Classifier::train_knn(CvMat *desc, CvMat *responses) 
{
    cout << "------------------------------------------" << endl;
    cout << "Step Start: knn" << endl; 
    
    if (KNN_ON) knn.train(desc, responses);
    else cout << "KNN is turned off" << endl;
    
    cout << "Step Done:  knn" << endl;
    return true;
}


bool Classifier::train_rtree(CvMat *desc, CvMat *responses) 
{
    cout << "------------------------------------------" << endl;
    cout << "Step Start: rtree" << endl; 
    
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
    
    // //printf("%d %d\n", NUM_CLUSTERS, desc->cols);
    // assert(NUM_CLUSTERS == desc->cols);
    // CvScalar avg, std;
    // cvAvgSdv(desc, &avg, &std);
    
    // printf("variance %f mean %f\n", std.val[0], avg.val[0]);
    
    CvMat *var_type = cvCreateMat(desc->cols + 1, 1, CV_8U);
    cvSet(var_type, cvScalarAll(CV_VAR_NUMERICAL));
    cvSet2D(var_type, desc->cols, 0, cvScalar(CV_VAR_CATEGORICAL));
    
    if (RTREE_ON) rtree.train(
        desc, 
        CV_ROW_SAMPLE,
        responses,
        0,
        0,
        var_type,
        0,
        params
    );
    else cout << "RTREE is turned off" << endl;
    
    // const CvMat *imp = rtree.get_var_importance();
    // if (!imp) cout << "can't get var importance" << endl;
    // else {
    
    // cout << "Var importance: " << endl;
    // for (int i = 0; i < imp->cols; i++)
        // printf("%f ", cvGetReal2D(imp, 0, i));
    // }
    // cout << endl;
    
    cout << "Step Done:  rtree" << endl;
    return true;
}

bool Classifier::train_alltrees(CvMat *desc, CvMat *responses) 
{
    cout << "------------------------------------------" << endl;
    cout << "Step Start: trees" << endl; 
    
    if (!ALL_TREES_ON) return false;
    
    CvMat* var_type = cvCreateMat( desc->cols + 1, 1, CV_8U );
    for (int j = 0; j < desc->cols; ++j)
        CV_MAT_ELEM(*var_type, unsigned char, j, 0) = CV_VAR_NUMERICAL;
    CV_MAT_ELEM(*var_type, unsigned char, desc->cols, 0) = CV_VAR_CATEGORICAL;    
    
    CvBoostParams params( 
        CvBoost::REAL, 
        100, 
        0.95, 
        2, 
        false, 
        NULL 
    );
    params.split_criteria = CvBoost::DEFAULT;    
    
    CvMat *new_responses = cvCreateMat(responses->rows, 1, CV_32SC1);
    for (int klass = 0; klass < kNumObjectTypes - 1; ++klass) {
        if(klass == kOther) continue;
        
        cout << "Training: " << classIntToString(klass) << endl;
        
        for (int i = 0; i < responses->rows; ++i) {
            int this_klass = CV_MAT_ELEM(*responses, int, i, 0);
            int new_klass = this_klass == klass ? klass : kOther;
            
            CV_MAT_ELEM(*new_responses, int, i, 0) = new_klass;
        }
        
        trees[klass].train(
            desc,               // data
            CV_ROW_SAMPLE,      // type
            new_responses,      // responses
            0,                  //
            0,                  //
            var_type,           // var_types
            0,                  // 
            params              // params
        );
    }
    
    //CvReleaseMat(&var_type);
    //CvReleaseMat(&new_responses);
    
    cout << "Step Done:  mugtree" << endl;
    return true;
}

        
// tp, tn, fp, fn
void logResult(int *data, int klass, int trueClass) {
    if (trueClass != kOther) // positive
        if (klass == trueClass) // true
            data[0] += 1; //tp
        else
            data[3] += 1; //fn
    else // negative
        if (klass == trueClass) //true
            data[1] += 1; //tn
        else
            data[2] += 1; //fp
}

void logResult(int *data, int klass, int trueClass, int targetClass) {
    if (trueClass == targetClass)
        if (klass == trueClass)
            data[0] += 1;
        else
            data[3] += 1;
    else
        if (klass == trueClass)
            data[1] += 1;
        else
            data[2] += 1;
}

void printResult(int *data, string name) {
    double accuracy = (double)(data[0] + data[1]) / (double)(data[0] + data[1] + data[2] + data[3]); 
    double precision = (double)data[0] / (double)(data[0] + data[2]);
    double recall = (double)data[0] / (double)(data[0] + data[3]);
    double fscore = 2 * (precision * recall) / (precision + recall);
    
    printf("%s\ttp: %d\ttn: %d\tfp: %d\tfn: %d\n", name.c_str(), data[0], data[1], data[2], data[3]);
    printf("%s\taccuracy: %f\tprecision: %f\trecall: %f\tfscore: %f\n", name.c_str(), accuracy, precision, recall, fscore);
}

bool Classifier::train_test(class_feat_vec_star *data, bool verbose)
{
    cout << "------------------------------------------" << endl;
    cout << "Step Start: test" << endl;
    
    int bayes_results[4], svm_results[4], knn_results[4], rtree_results[4], alltrees_results[4];
    for (int i = 0; i < 4; ++i)
        bayes_results[i] = svm_results[i] = knn_results[i] = rtree_results[i] = alltrees_results[i] = 0;

    cout << "Running " << (int)(*data).size() << " images." << endl;
    for (int img = 0; img < (int)(*data).size(); ++img) {
        showProgress(img, (int)(*data).size());
        
        vector<feat> pts = (*data)[img]->second;
        vector<int> cluster(pts.size());
        for (int i = 0; i < (int)pts.size(); ++i) {
            float min_distance = INT_MAX;
            int min_index = -1;
            for (int c = 0; c < NUM_CLUSTERS; ++c) {
                float dist = 0;
                for (int j = 0; j < SURF_SIZE; ++j) {
                    dist += pow(pts[i][j] - cvGetReal2D(centers, c, j), 2);
                }
                
     //           cout << pts[i][0] << " " << cvGetReal2D(centers, c, 0) << " " << dist << endl;
                
                if (dist < min_distance) {
                    min_index = c;
                    min_distance = dist;
                }
            }
            
            cluster[i] = min_index;
        }
        
        CvMat *query = cvCreateMat(1, NUM_CLUSTERS, CV_32FC1);
        cvSet(query, cvScalar(0));
        for (int row = 0; row < (int)pts.size(); ++row) {
            int cluster_idx = cluster[row];

            int oldVal = cvGetReal2D(query, 0, cluster_idx);
            cvSetReal2D(query, 0, cluster_idx, oldVal+1);//cvScalar(oldVal+1));
        }
        
        // double sum = 0;
        // for (int j = 0; j < NUM_CLUSTERS; ++j)
            // sum += cvGetReal2D(query, 0, j);
        
        // for (int j = 0; j < NUM_CLUSTERS; ++j) {
            // double val = cvGetReal2D(query, 0, j);
            // double newval = val / sum;
            // cvSetReal2D(query, 0, j, newval);
        // }    
        
        int trueClass = stringToClassInt((*data)[img]->first);
        
        int klass, klass2, klass3, klass4, klass5;
        double scores[kNumObjectTypes - 1];
        
        if (BAYES_ON) klass = bayes.predict(query);
        if (SVM_ON) klass2 = svm.predict(query);
        if (KNN_ON) klass3 = knn.find_nearest(query, 5);
        if (RTREE_ON) klass4 = rtree.predict(query);

        if (BAYES_ON) logResult(bayes_results, klass, trueClass);
        if (SVM_ON) logResult(svm_results, klass2, trueClass);
        if (KNN_ON) logResult(knn_results, klass3, trueClass);
        if (RTREE_ON) logResult(rtree_results, klass4, trueClass);
        //cout << ALL_TREES_ON << "*" << endl;
        if (ALL_TREES_ON) {
            cout << "avg: " << cvAvg(query).val[0] << endl;
            for (int klass = 0; klass < kNumObjectTypes - 1; ++klass) {
                int length = cvSliceLength(CV_WHOLE_SEQ, trees[klass].get_weak_predictors());
                CvMat *weakResponses = cvCreateMat(length, 1, CV_32FC1);
                klass5 = trees[klass].predict(query, NULL, weakResponses, CV_WHOLE_SEQ);
                
                //cout << klass5 << "**" << endl;
                
                double score = cvSum(weakResponses).val[0];
                scores[klass] = score;
                
                cvReleaseMat(&weakResponses);
            }
                         
            double min_score = 0;
            int max_klass = -1;
            for (int klass = 0; klass < kNumObjectTypes - 1; ++klass) {
                if (scores[klass] < min_score) {
                    max_klass = klass;
                    min_score = scores[klass];
                }
            }
            
            klass5 = min_score < 0 ? max_klass : kOther;
            logResult(alltrees_results, klass5, trueClass);
        }
        
        if (verbose) {
            cout << (*data)[img]->first << " (" << trueClass << ") is a ";
            if (BAYES_ON) printf(" %s (bayes)!", classIntToString(klass).c_str());
            if (SVM_ON)   printf(" %s (svm)!", classIntToString(klass2).c_str());
            if (KNN_ON)   printf(" %s (knn)!", classIntToString(klass3).c_str());
            if (RTREE_ON)   printf(" %s (rtree)!", classIntToString(klass4).c_str());
            if (ALL_TREES_ON) { 
                    cout << "\ntrees: " << classIntToString(klass5) << endl;
                for (int klass = 0; klass < kNumObjectTypes - 1; ++klass) {
                    cout << classIntToString(klass) << " (" << scores[klass] << ") ";
                }
                cout << endl;
            }
            cout << endl;
        }
    }
    
    cout << endl;
    
    if (BAYES_ON) printResult(bayes_results, "bayes");
    if (SVM_ON) printResult(svm_results, "svm");
    if (RTREE_ON) printResult(rtree_results, "rtree");
    if (KNN_ON) printResult(knn_results, "knn");
    if (ALL_TREES_ON) printResult(alltrees_results, "alltrees");
    
    cout << "Step Done:  test" << endl;
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
    
    int maxOther = 4000;
    int maxImages = INT_MAX;

    cout << "Processing images..." << endl;

    //vector<int> numIpoints(kNumObjectTypes);
    int minfiles = min(maxImages, (int)fileList.files.size());
    for (int i = 0; i < minfiles; i++) 
    {
        // skip too many others...
        if (fileList.files[i].label == "other" && --maxOther < 0) continue;

        // show progress
        if (i % 10 == 0) showProgress(i, minfiles);

        image = cvLoadImage(fileList.files[i].filename.c_str(), 0);
        
        if (image == NULL) {
            cerr << "ERROR: could not load image " << fileList.files[i].filename.c_str() << endl;
            continue;
        }
        
        // const int MAX_CORNERS = 500;
        
          // /* IVAN'S CODE - extract corners! */
        // CvSize img_sz = cvGetSize(image);
        // //IplImage *image = cvCreateImage(img_sz, IPL_DEPTH_8U, 1);
        // //cvCvtColor(frame, image, CV_BGR2GRAY);
        // IplImage* eig_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );
        // IplImage* tmp_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );
        // int corner_count = MAX_CORNERS;
        // CvPoint2D32f* cornersA = new CvPoint2D32f[ MAX_CORNERS ];
        // cvGoodFeaturesToTrack(
            // image,
            // eig_image,
            // tmp_image,
            // cornersA,
            // &corner_count,
            // 0.01,
            // 5.0,
            // 0,
            // 3,
            // 0,
            // 0.04
        // );
        // cvFindCornerSubPix(
            // image,
            // cornersA,
            // corner_count,
            // cvSize(10, 10),
            // cvSize(-1, -1),
            // cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,20,0.03)
        // );
    
        // CvMat *pts = cvCreateMat(corner_count, 2, CV_32F);
        // for (int row = 0; row < corner_count; ++row) {
            // CV_MAT_ELEM(*pts, float, row, 0) = cornersA[row].x;
            // CV_MAT_ELEM(*pts, float, row, 1) = cornersA[row].y;
        // }
        
        // points.push_back(pts);
        
        // CvMat *cvhomog = cvCreateMat(3, 3, CV_32F);
        // const double MAX_ERR = 10;
        // int best = -1;
        // double bestscore = DBL_MAX;
        // for (int j = 0; j < (int)points.size() - 1; ++j) {
            // int tries = 10;
            // double error = DBL_MAX;
            // CvMat *otherpts = points[j];
            // int max_pts = min(pts->rows, otherpts->rows);
            // if (max_pts < 8) continue;
            
            // vector<int> which1(max_pts), which2(max_pts);
            // for (int k = 0; k < max_pts; ++k) which1[k] = which2[k] = k;
            // while(error > MAX_ERR && --tries > 0) {
                // int choose = 8 + rand() % (max_pts - 8);
                // CvMat *newpts1 = cvCreateMat(choose, 2, CV_32F);
                // CvMat *newpts2 = cvCreateMat(choose, 2, CV_32F);

                // random_shuffle(which1.begin(), which1.end());
                // random_shuffle(which2.begin(), which2.end());
                
                // for (int k = 0; k < choose; ++k) {
                    // CV_MAT_ELEM(*newpts1, float, k, 0) = CV_MAT_ELEM(*pts, float, which1[k], 0);
                    // CV_MAT_ELEM(*newpts1, float, k, 1) = CV_MAT_ELEM(*pts, float, which1[k], 1);
                    // CV_MAT_ELEM(*newpts2, float, k, 0) = CV_MAT_ELEM(*otherpts, float, which2[k], 0);
                    // CV_MAT_ELEM(*newpts2, float, k, 1) = CV_MAT_ELEM(*otherpts, float, which2[k], 1);
                // }
                
                // cvFindHomography(newpts1, newpts2, cvhomog, CV_RANSAC, 2);
                
                // if (abs(cvAvg(cvhomog).val[0]) < bestscore) {
                    // best = j;
                    // bestscore = abs(cvAvg(cvhomog).val[0]);
                // }
            // }
        // }
        
       // cout << "best: " << best << endl;
        // Homography h;
        // CvMat *homog = h.compute(pts, pts);
        // cvReleaseMat(&pts);
        
        // cout << cvAvg(homog).val[0] << endl;
        
        //CvMat *cvhomog = cvCreateMat(3, 3, CV_32F);
        //cvFindHomography(pts, pts, cvhomog, CV_RANSAC, 2);
        //cout << cvAvg(cvhomog).val[0] << endl;
        
        string label = fileList.files[i].label;

        CvSeq *keypoints = 0, *descriptors = 0;
        CvSURFParams params = cvSURFParams(100, SURF_SIZE == 128);
        cvExtractSURF(image, 0, &keypoints, &descriptors, storage, params);
        
        if (descriptors->total == 0) continue;
        
        vector<float> desc;
        desc.resize(descriptors->total * descriptors->elem_size/sizeof(float));
        cvCvtSeqToArray(descriptors, &desc[0]);
        
        vector<feat> features;
        int where = 0;
        for (int pt = 0; pt < keypoints->total; ++pt) {
            float *f = new float[SURF_SIZE];
            for (int j = 0; j < SURF_SIZE; ++j) {
                f[j] = desc[where];
                ++where;
            }
            features.push_back(f);
        }
        
        allImages.push_back(class_feat(label, features));
    }
    
    cout << endl << "Extracted surf features: " << endl;

    return true;
}


