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
 
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
 
#include "classifier.h"
#include "logreg.h"
 
 
// Classifier class ---------------------------------------------------------
 
// default constructor
Classifier::Classifier()
{
    // initalize the random number generator (for sample code)
    rng = cvRNG(-1);
 
    _features.load("dict.xml");
    _regressor = new LogReg(_features.numFeatures(), double(0.01));
    
}
    
// destructor
Classifier::~Classifier()
{
    delete _regressor;
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
    int i = 0;
    while(!infile.fail() && !infile.eof()) {
        infile >> val;
        if (infile.fail() || infile.eof()) return true;
        
        _regressor->set(i++, val);
    }
    
    return true;
}
 
// saveState
// Writes classifier configuration to the given file
bool Classifier::saveState(const char *filename)
{
    assert(filename != NULL);
    
    ofstream outfile;
    outfile.open(filename); //<= because of bias.
    for (unsigned i = 0; i <= _features.numFeatures(); i++) {
        outfile << _regressor->get(i) << ' ';
    }
    outfile.close();
    
    return true;
}

bool Classifier::loadTrainingFile(const char *filename, std::vector<Trainer> *trainers)
{
    if (filename == NULL) return false;
    
    ifstream infile;
    infile.open(filename);
    
    int vars, count;
    
    infile >> count;
    if (infile.fail() || infile.eof()) return false;
    
    infile >> vars;
    if (infile.fail() || infile.eof()) return false;    
    
    for (int i = 0; i < count; i++) 
    {
    
	    infile.ignore(1);
	    
        Trainer t;
        infile >> t.truth;
     	
     	t.values = new double[vars + 1];
        
        double val;
        for (int j = 0; j <= vars; j ++) 
        {
            infile >> t.values[j];
        }
        trainers->push_back(t);
    }
    
    cout << "Loaded training feature values from: " << filename << endl;
    
    return true;
}
 
// saveState
// Writes classifier TrainingFile to the given file
bool Classifier::saveTrainingFile(const char *filename, std::vector<Trainer> *trainers)
{
    if (filename == NULL) return false;
    
    ofstream outfile;
    outfile.open(filename); //<= because of bias.
    
    outfile << trainers->size() << ' ' << _features.numFeatures() << endl;
    
    for (unsigned i = 0; i < trainers->size(); i++) 
    {
        outfile << (*trainers)[i].truth << ' ';
        for (unsigned j = 0; j <= _features.numFeatures(); j++) 
        {
            outfile << (*trainers)[i].values[j] << ' ';
        }
        outfile << endl;
    }
    outfile.close();
    
    return true;
}

// TESTING ONLY
bool showRect(const IplImage *image, CObject *rect) {
    //char *WINDOW_NAME = "test";
    CvFont font;

    cvNamedWindow("test", CV_WINDOW_AUTOSIZE);
    cvInitFont(&font, CV_FONT_VECTOR0, 0.75, 0.75, 0.0f, 1, CV_AA);
    
    IplImage *frameCopy = cvCloneImage(image);
    rect->draw(frameCopy, CV_RGB(255, 0, 255), &font);
    
    cvShowImage("test", frameCopy);
    cvReleaseImage(&frameCopy);

    return cvWaitKey(50) != -1;
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
  int numLayers = 7;
  TemplateMatcher tm;
  
  int absBestX = INT_MIN, absBestY = INT_MIN;
  double absBestScore = INT_MIN, absBestScale = scale;
  
  while(numLayers-- > 0) {
    bool skipped = false;
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
        
        //cout << featureMap->width << " ";
        
        // Maintain the furthest we can slide the 32x32 window.
        if (featureMap->width < maxWidth) maxWidth = featureMap->width;
        if (featureMap->height < maxHeight) maxHeight = featureMap->height;
    }
    
    // Compute sum of theta(i) x(i).
    int bestX = INT_MIN, bestY = INT_MIN;
    double bestScore = INT_MIN;
    
    // TODO: figure out.
    maxWidth = dst->width;
    maxHeight = dst->height;
    
    for (int x = 0; x < maxWidth; x += 8) {
        for (int y = 0; y < maxHeight; y += 8) {
        
            double score = 0;
            bool bad = false;
            for (unsigned featureNum = 0; featureNum < _features.numFeatures(); featureNum++) {
                FeatureDefinition *fd = _features.getFeature(featureNum);
                CvRect r = fd->getValidRect();
                r.x += x;
                r.y += y;
                IplImage *featureMap = images[featureNum];
                
                if (r.x + r.width > featureMap->width || r.y + r.height > featureMap->height) {
                    bad = true;
                    break;
                }
                
                double value = this->maxpool(featureMap, r); 
                score += value * _regressor->get(featureNum);
                
                /*if (!bad && !skipped) {
                    CObject obj;
                    obj.rect = r;
                    char buffer[50];
                    sprintf(buffer, "%f", value * _regressor->get(featureNum));
                    obj.label = buffer;
                    bool did_skip = showRect(dst, &obj);
                    if (did_skip) skipped = true;
                }*/
            }
            
            // Add bias.
            score += _regressor->get(_features.numFeatures());
            
            /*if (!bad && !skipped) {
                CObject obj;
                obj.rect = cvRect(x * scale, y * scale, 32 * scale, 32 * scale);
                char buffer[50];
                sprintf(buffer, "%f", score);
                obj.label = buffer;
                bool did_skip = showRect(gray, &obj);
                if (did_skip) skipped = true;
            }*/
            
            //cout << "x: " << x << " y: " << y << " score: " << score << endl;
            if (!bad && score > bestScore) {
                bestScore = score;
                bestX = x;
                bestY = y;
            }
        }
    }

    // Free old images.
    for (unsigned imageNum = 0; imageNum < images.size(); imageNum ++) {
        cvReleaseImage(&images[imageNum]);
    }
    
    if (bestScore > absBestScore) {
        absBestScore = bestScore;
        absBestX = bestX;
        absBestY = bestY;
        absBestScale = scale;
    }
    
    // Do new resize.
    scale *= 1.2;
    IplImage *old = dst;
    
    CvSize newSize = cvSize(gray->width / scale, gray->height / scale);
    dst = cvCreateImage(newSize, gray->depth, gray->nChannels);
    cvResize(gray, dst);
    
    cvReleaseImage(&old);
  }
  
  // Logistic regression.
    double logScore = 1.0 / (1.0 + exp(-1.0 * absBestScore));

    cout << "score: " << absBestScore << " logScore: " << logScore << endl;

    // If logScore > 0.5, we have a match!
    if (logScore > 0.5) {
        CObject obj;
        obj.rect = cvRect(absBestX * absBestScale, absBestY * absBestScale, 32 * absBestScale, 32 * absBestScale);
        obj.label = "mug";
        objects->push_back(obj);
    }
  
  cvReleaseImage(&gray);
  return true;
}

double Classifier::maxpool(IplImage *r, const CvRect &pool)
{
    float max = -1.0;
    
    for (int x = pool.x; x < pool.x + pool.width; x++) {
        for (int y = pool.y; y < pool.y + pool.height; y++) {
            float val = CV_IMAGE_ELEM(r, float, y, x);
            if ( val > max)
                max = val;
        }
    }
    
    return max;
}
 
double *Classifier::feature_values(IplImage *dst, TemplateMatcher *tm)
{
    tm->loadFrame(dst);
    FeatureDefinition *fd = NULL;
    
    double *values = new double[_features.numFeatures()];
    for (unsigned i = 0; i < _features.numFeatures(); i++) {
        
        fd = _features.getFeature(i);
        
        CvSize newSize = cvSize(dst->width - fd->getTemplateWidth() + 1, dst->height - fd->getTemplateHeight() + 1);
        IplImage *response = cvCreateImage(newSize, IPL_DEPTH_32F, 1);
        
        tm->makeResponseImage(fd->getTemplate(), response);
        CvRect valid = fd->getValidRect();
        values[i] = this->maxpool(response, valid);

        cvReleaseImage(&response);
        
    }
    return values; // REMEMBER TO FREE!
}
 
// train
// Trains the classifier to recognize the objects given in the
// training file list.
bool Classifier::train(TTrainingFileList& fileList, const char *trainingFile)
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
    
    std::vector<Trainer> values;
    TemplateMatcher tm;
 
    int maxMugs = INT_MAX;
    int maxOther = INT_MAX;
 
    
    bool extractVector = !this->loadTrainingFile(trainingFile, &values);
    
    if (extractVector) {
    
	    cout << "Processing images..." << endl;
	    smallImage = cvCreateImage(cvSize(32, 32), IPL_DEPTH_8U, 1);
    
        for (int i = 0; i < (int)fileList.files.size(); i++) {
            // show progress
            if (i % 10 == 0) showProgress(i, fileList.files.size());
 
            // skip non-mug and non-other images (milestone only)
            if ((fileList.files[i].label == "mug") ||
                (fileList.files[i].label == "other")) {
            
                // Only use so many of each...
                if (fileList.files[i].label == "mug") {
                    if (maxMugs-- < 0) continue;
                } else if (fileList.files[i].label == "other") {
                    if (maxOther-- < 0) continue;
                }
            
                // load the image
                image = cvLoadImage(fileList.files[i].filename.c_str(), 0);
                if (image == NULL) {
                    cerr << "ERROR: could not load image " << fileList.files[i].filename.c_str() << endl;
                    continue;
                }
            
                // resize to 32 x 32
                cvResize(image, smallImage);
            
                Trainer t;
                t.values = this->feature_values(smallImage, &tm);
                t.truth = (fileList.files[i].label == "mug");
            
                values.push_back(t);
            
                // free memory
                cvReleaseImage(&image);
            }
        }
        
        // free memory
        cvReleaseImage(&smallImage);
        cout << endl;

        this->saveTrainingFile(trainingFile, &values);
    }
    
    // TRAINING!
    
    cout << "Training to be a pro! (We need a montage) " << endl << "* * * 80s Music begins playing... * * *" << endl;
    
    double diff = 1;
    for (int e = 0; e < 1000 && diff > 0.001; e++)
    {
        for (unsigned int i = 0; i < values.size(); i++)
        {
            _regressor->add_to_batch(values[i]); // Train on one set of scores. All features on one training image.
        }
        diff = fabs(_regressor->gradient_decent()); // Process batch (gradient vector)
    }
    
    cout << endl << endl << "All read up. Now, burn the books!" << endl;
    
    for (unsigned int i = 0; i < values.size(); i++)
    {
        delete[] values[i].values;
    }
    
    cout << endl << endl << "I'm ready. Let's DO THIS!" << endl;
    
    return true;
}
