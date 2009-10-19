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

using namespace std;

// Classifier class ---------------------------------------------------------

// default constructor
Classifier::Classifier()
{
    // initalize the random number generator (for sample code)
    rng = cvRNG(-1);

    _features.load("dict.xml");
    

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
    
    // CS221 TO DO: replace this with your own configuration code
    
    return true;
}

// saveState
// Writes classifier configuration to the given file
bool Classifier::saveState(const char *filename)
{
    assert(filename != NULL);
    
    // CS221 TO DO: replace this with your own configuration code
    
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
  
  // CS221 TO DO: replace this with your own code
  
  // Example code which returns up to 10 random objects, each object
  // having a width and height equal to half the frame size.
  const char *labels[5] = {
    "mug", "stapler", "keyboard", "clock", "scissors"
  }; 
  int n = cvRandInt(&rng) % 10;
  while (n-- > 0) {
    CObject obj;
    obj.rect = cvRect(0, 0, frame->width / 2, frame->height / 2);
    obj.rect.x = cvRandInt(&rng) % (frame->width - obj.rect.width);
    obj.rect.y = cvRandInt(&rng) % (frame->height - obj.rect.height);
    obj.label = string(labels[cvRandInt(&rng) % 5]);
    objects->push_back(obj);        
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
