/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2009, Stanford University
**
** FILENAME:    featureDictionary.h
** AUTHOR(S):   Ian Goodfellow <ia3n@stanford.edu>
** DESCRIPTION:
**  See featureDictionary.cpp
**
*****************************************************************************/

#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <limits>
#include <algorithm>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "featureDictionary.h"

#if defined(_WIN32)||defined(WIN32)||defined(__WIN32__)
#undef max
#endif

using namespace std;

// static prototype
static void scaleToRange(CvArr *array, double minValue, double maxValue);

FeatureDictionary::FeatureDictionary(unsigned width, unsigned height)
{
    _windowSize = cvSize(width, height);
}

FeatureDictionary::FeatureDictionary(const FeatureDictionary& d)
{
  _windowSize = d._windowSize;


  for (unsigned i = 0; i < d._entries.size(); i++)
    {
      _entries.push_back(d._entries[i]->clone());
    }
}

FeatureDictionary::~FeatureDictionary()
{
  for (unsigned i = 0; i < _entries.size(); i++)
    delete _entries[i];
}

bool FeatureDictionary::load(const char *filename)
{
  XMLNode root = XMLNode::parseFile(filename);


    bool rval = load(root);
    if(!rval)
      cerr <<  "could not read feature dictionary file " << endl;
    return rval;
}

bool FeatureDictionary::load(XMLNode &root)
{
    _entries.clear();

    if (root.isEmpty()) {
      cerr <<  "Feature dictionary file is missing or has incorrect root." << endl;
      return false;
    }

    if (string(root.getName()) != "FeatureDictionary" ) {
      cerr <<  "Attempt to read feature dictionary from XMLNode that is not a feature dictionary." << endl;
	return false;
    }

    _windowSize.width = atoi(root.getAttribute("width"));
    _windowSize.height = atoi(root.getAttribute("height"));
    int n = atoi(root.getAttribute("numEntries"));


    assert(root.nChildNode("FeatureDefinition") == n);
    _entries.resize(n, NULL);

    for (unsigned i = 0; i < _entries.size(); i++) 
    {		
        XMLNode node = root.getChildNode("FeatureDefinition", i);
        _entries[i] = new FeatureDefinition(node);
        assert(_entries[i] != NULL);
    }

    return true;
}

IplImage *FeatureDictionary::visualizeDictionary() const
{
    int n = (int)ceil(sqrt((double)_entries.size()));
    IplImage *allFeaturees = cvCreateImage(cvSize(n * _windowSize.width, n * _windowSize.height),
        IPL_DEPTH_8U, 3);
    cvZero(allFeaturees);

    for (unsigned i = 0; i < _entries.size(); i++) {		
	CvRect r = cvRect((i % n) * _windowSize.width, ((int)(i / n)) * _windowSize.height,
            _windowSize.width, _windowSize.height);
	cvRectangle(allFeaturees, cvPoint(r.x, r.y), cvPoint(r.x + r.width, r.y + r.height),
            CV_RGB(0, 0, 255), 1);

	cvSetImageROI(allFeaturees, cvRect(r.x + _entries[i]->_validRect.x, r.y + _entries[i]->_validRect.y,
		_entries[i]->_template->width, _entries[i]->_template->height));
	IplImage *templateCopy = cvCloneImage(_entries[i]->_template);

	if (templateCopy->nChannels != 1) {
	  IplImage *grayTemplateCopy = cvCreateImage(cvGetSize(templateCopy), templateCopy->depth, 1);
	  cvCvtColor(templateCopy, grayTemplateCopy, CV_BGR2GRAY);
	  cvReleaseImage(&templateCopy);
	  templateCopy = grayTemplateCopy;
	}

	scaleToRange(templateCopy, 0.0, 255.0);
        if (templateCopy->depth != IPL_DEPTH_8U) {
            IplImage *tmpImg = cvCreateImage(cvGetSize(templateCopy), IPL_DEPTH_8U, 1);
            cvConvertScale(templateCopy, tmpImg);
            cvReleaseImage(&templateCopy);
            templateCopy = tmpImg;
        }
	for (unsigned k = 0; k < 3; k++) 
	{
	    cvSetImageCOI(allFeaturees, k + 1);
	    //cvCopyImage(_entries[i]->_template, allFeaturees);
	    cvCopyImage(templateCopy, allFeaturees);
	}
	cvReleaseImage(&templateCopy);
	cvSetImageCOI(allFeaturees, 0);
	cvResetImageROI(allFeaturees);

	cvRectangle(allFeaturees, cvPoint(r.x + _entries[i]->_validRect.x, r.y + _entries[i]->_validRect.y),
            cvPoint(r.x + _entries[i]->_validRect.x + _entries[i]->_validRect.width + _entries[i]->_template->width,
                r.y + _entries[i]->_validRect.y + _entries[i]->_validRect.height + _entries[i]->_template->height),
            CV_RGB(255, 0, 0), 1);
    }

    return allFeaturees;
}

IplImage *FeatureDictionary::visualizeFeature(unsigned index, IplImage *image) const
{
    // create image of the right size (unless one has been passed in)
    if ((image == NULL) || (image->width != _windowSize.width) ||
	(image->height != _windowSize.height) || (image->depth != IPL_DEPTH_8U) ||
	(image->nChannels != 3)) {
	if (image != NULL) {
	    cvReleaseImage(&image);
	}
	image = cvCreateImage(cvSize(_windowSize.width, _windowSize.height),
	    IPL_DEPTH_8U, 3);
    }
    cvZero(image);
    
    // copy template into image
    cvSetImageROI(image, cvRect(_entries[index]->_validRect.x, _entries[index]->_validRect.y,
	    _entries[index]->_template->width, _entries[index]->_template->height));

    IplImage *templateCopy = cvCloneImage(_entries[index]->_template);
    if (templateCopy->nChannels != 1) {
      IplImage *grayTemplateCopy = cvCreateImage(cvGetSize(templateCopy), templateCopy->depth, 1);
      cvCvtColor(templateCopy, grayTemplateCopy, CV_BGR2GRAY);
      cvReleaseImage(&templateCopy);
      templateCopy = grayTemplateCopy;
    }

    scaleToRange(templateCopy, 0.0, 255.0);
    if (templateCopy->depth != IPL_DEPTH_8U) {
        IplImage *tmpImg = cvCreateImage(cvGetSize(templateCopy), IPL_DEPTH_8U, 1);
        cvConvertScale(templateCopy, tmpImg);
        cvReleaseImage(&templateCopy);
        templateCopy = tmpImg;
    }

    for (unsigned k = 0; k < 3; k++) {
	cvSetImageCOI(image, k + 1);
	cvCopyImage(templateCopy, image);
    }
    
    cvReleaseImage(&templateCopy);
    cvSetImageCOI(image, 0);
    cvResetImageROI(image);
    
    // draw box around valid region
    cvRectangle(image, cvPoint(_entries[index]->_validRect.x, _entries[index]->_validRect.y),
	cvPoint(_entries[index]->_validRect.x + _entries[index]->_validRect.width + _entries[index]->_template->width,
	    _entries[index]->_validRect.y + _entries[index]->_validRect.height + _entries[index]->_template->height),
	CV_RGB(255, 0, 0), 1);
    
    int j = 0;
    CvPoint pt =  cvPoint(_entries[index]->_validRect.x + _entries[index]->_validRect.width + _entries[index]->_template->width,
			  _entries[index]->_validRect.y + _entries[index]->_validRect.height + _entries[index]->_template->height);
    cvLine(image, cvPoint(pt.x - 2 * (j + 1), pt.y - 2), cvPoint(pt.x - 2 * (j + 1), pt.y - 3), CV_RGB(255, 0, 0)); 
    
    
    return image;
}


static void scaleToRange(CvArr *array, double minValue, double maxValue)
{
    assert((array != NULL) && (minValue <= maxValue));

    double m, M;
    cvMinMaxLoc(array, &m, &M);
    if (m != M) {
	cvScale(array, array, (maxValue - minValue) / (M - m), 
            minValue - m * (maxValue - minValue) / (M - m));
    }
}

