/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    classifier.cpp
** AUTHOR(S):   Stephen Gould <sgould@stanford.edu>
**              Ian Goodfellow <ia3n@stanford.edu>
** DESCRIPTION:
**  See classifier.h
**
*****************************************************************************/

#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "classifier.h"
#include "template.h"

using namespace std;

// Classifier class ---------------------------------------------------------

// default constructor
Classifier::Classifier()
{
    // initalize the random number generator (for sample code)
    rng = cvRNG(-1);

    _features.load("dict.xml");
    mugFeatures = std::vector<double>(_features.numFeatures());
    
    for (unsigned i = 0; i < mugFeatures.size(); i++) {
        mugFeatures[i] = 0;
    }
    mugFeatures[5] = 100;

    // CS221 TO DO: add initialization for any member variables   
}
    
// destructor
Classifier::~Classifier()
{

    // CS221 TO DO: free any memory allocated by the object
}

// loadState
// Configure the classifier from the given file.
bool Classifier::loadState(const char *filename)
{
    assert(filename != NULL);
    
    ifstream infile;
    infile.open(filename);
    
    double val;
    while(!infile.fail() && !infile.eof()) {
        infile >> val;
        if (infile.fail() || infile.eof()) return true;
        
        mugFeatures.push_back(val);
    }
    
    return true;
}

// saveState
// Writes classifier configuration to the given file
bool Classifier::saveState(const char *filename)
{
    assert(filename != NULL);
    
    ofstream outfile;
    outfile.open(filename);
    for (unsigned i = 0; i < mugFeatures.size(); i++) {
        outfile << mugFeatures[i] << ' ';
    }
    outfile.close();
    
    return true;
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
  if (!scored)
    return true;

  assert((frame != NULL) && (objects != NULL));
  
  // Convert to grayscale.
  IplImage *gray = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
  cvCvtColor(frame, gray, CV_BGR2GRAY);
  
  // Resize by half first, as per the handout.
  double scale = 2.0;
  IplImage *dst = cvCreateImage(cvSize(gray->width / scale, gray->height / scale), gray->depth, gray->nChannels);
  cvResize(gray, dst);
  
  // Do six further resizes, evaluating each time.
  int numLayers = 6;
  TemplateMatcher tm;
  while(numLayers-- > 0) {
    tm.loadFrame(dst);
    
    // Store feature maps.
    std::vector<IplImage *> images;
    int maxWidth = INT_MAX, maxHeight = INT_MAX;
    for (unsigned featureNum = 0; featureNum < _features.numFeatures(); featureNum++) {
        FeatureDefinition *fd = _features.getFeature(featureNum);
        CvRect r = fd->getValidRect();
        
        CvSize newSize = cvSize(dst->width - fd->getTemplateWidth() + 1, dst->height - fd->getTemplateHeight() + 1);
        IplImage *featureMap = cvCreateImage(newSize, IPL_DEPTH_32F, 1);
        tm.makeResponseImage(fd->getTemplate(), featureMap);
        images.push_back(featureMap);
        
        // Maintain the furthest we can slide the 32x32 window.
        if (featureMap->width < maxWidth) maxWidth = featureMap->width;
        if (featureMap->height < maxHeight) maxHeight = featureMap->height;
    }
    
    // Compute sum of theta(i) x(i).
    int bestX, bestY;
    double bestScore = -1;
    for (int x = 0; x < maxWidth - 1; x += 8) {
        for (int y = 0; y < maxHeight - 1; y += 8) {
        
            double score = 0;
            for (unsigned featureNum = 0; featureNum < _features.numFeatures(); featureNum++) {
                FeatureDefinition *fd = _features.getFeature(featureNum);
                CvRect r = fd->getValidRect();
                IplImage *featureMap = images[featureNum];
                
                // TODO: max pooling.
                double value = cvGetReal2D(featureMap, y + r.y, x + r.x);
                score += value * mugFeatures[featureNum];
            }
            
            if (score > bestScore) {
                bestScore = score;
                bestX = x;
                bestY = y;
            }
        }
    }
    
    // Logistic regression.
    double logScore = 1.0 / (1.0 + exp(-1.0 * bestScore));
    
    // If logScore > 0.5, we have a match!
    if (logScore > 0.5) {
        CObject obj;
        obj.rect = cvRect(bestX, bestY, 32 * scale, 32 * scale);
        obj.label = "mug";
        objects->push_back(obj); 
    }
    
    // Do new resize.
    scale *= 1.2;
    IplImage *old = dst;
    
    CvSize newSize = cvSize(gray->width / scale, gray->height / scale);
    dst = cvCreateImage(newSize, gray->depth, gray->nChannels);
    cvResize(gray, dst);
    
    cvReleaseImage(&old);
  }
  
  return true;
}

/*
bool Classifier::train(TTrainingFileList& fileList)
{
    cout << "Training to be a champ!" << endl;
    
    //TODO JUAN -- add LogReg use here!
    
    cout << "I'm ready. Let's DO THIS!" << endl;
}
*/

// train
// Trains the classifier to recognize the objects given in the
// training file list.
bool Classifier::train(TTrainingFileList& fileList)
{
    // CS221 TO DO: replace with your own training code

    // example code to show you number of samples for each object class
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

    // example code for loading and resizing image files--
    // you may find this useful for the milestone    
    IplImage *image, *smallImage;

    cout << "Processing images..." << endl;
    smallImage = cvCreateImage(cvSize(64, 64), IPL_DEPTH_8U, 1);
    for (int i = 0; i < (int)fileList.files.size(); i++) {
        // show progress
        if (i % 1000 == 0) {
            showProgress(i, fileList.files.size());
        }

        // skip non-mug and non-other images (milestone only)
        if ((fileList.files[i].label == "mug") ||
            (fileList.files[i].label == "other")) {
            
            // load the image
            image = cvLoadImage(fileList.files[i].filename.c_str(), 0);
            if (image == NULL) {
                cerr << "ERROR: could not load image "
                     << fileList.files[i].filename.c_str() << endl;
                continue;
            }

            // resize to 64 x 64
            cvResize(image, smallImage);

            // CS221 TO DO: extract features from image here

            // free memory
            cvReleaseImage(&image);
        }
    }

    // free memory
    cvReleaseImage(&smallImage);
    cout << endl;

    // CS221 TO DO: train you classifier here

    return true;
}
