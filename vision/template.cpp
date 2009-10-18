/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2009, Stanford University
**
** FILENAME:    template.cpp
** AUTHOR(S):   Ian Goodfellow <ia3n@stanford.edu>
**              Adam Coates <acoates@stanford.edu>
** DESCRIPTION:
**  See template.h
**
*****************************************************************************/

#include "template.h"

#include <iostream>
using namespace std;

TemplateMatcher::TemplateMatcher()
{
  _img = _intImg = _intImgSqr = NULL;
}

TemplateMatcher::~TemplateMatcher()
{
  if (_intImg)
    cvReleaseImage(&_intImg);
  if (_intImgSqr)
    cvReleaseImage(&_intImgSqr);
}

void TemplateMatcher::loadFrame(const IplImage * img)
{
  _img = img;

  CvRect roi = cvGetImageROI(_img);

  bool allocate = true;
  if (_intImg)
    {
      if (_intImg->width != roi.width+1 || _intImg->height != roi.height +1)
	{
	  cvReleaseImage(&_intImg);
	}	
      else
	allocate = false;
    }
  if (allocate)
    _intImg = cvCreateImage(cvSize(roi.width+1, roi.height+1), IPL_DEPTH_64F, 1);
  
  allocate = true;
  if (_intImgSqr)
    {
      if (_intImgSqr->width != roi.width+1 || _intImgSqr->height != roi.height +1)
	{
	  cvReleaseImage(&_intImgSqr);
	}	
      else
	allocate = false;
    }
  if (allocate)
    _intImgSqr = cvCreateImage(cvSize(roi.width+1, roi.height+1), IPL_DEPTH_64F, 1); 

  cvIntegral(_img, _intImg, _intImgSqr);
}

void TemplateMatcher::makeResponseImage(const IplImage* t, IplImage* response) const
{
   CvRect roi = cvGetImageROI(_img);
   CvRect troi = cvGetImageROI(t);

   if (roi.width - t->width + 1 != response->width ||
       roi.height - t->height + 1 != response->height) {
     
     cerr << "Mismatched template, image, response sizes." << endl;
     assert(false);
   }
   if (troi.width != t->width || troi.height != t->height) {
     cerr <<  "Template has ROI set but it will be ignored." << endl;
   }

   // compute cross correlation for entire image.
   cvMatchTemplate(_img, t, response, CV_TM_CCORR);

   // compute patch normalization values
   double tMean, tVar;
   CvScalar tMean_, tVar_;
   cvAvgSdv(t, &tMean_, &tVar_);
   double N = t->width*t->height;
   tMean = tMean_.val[0]; tVar = N*tVar_.val[0]*tVar_.val[0];

   // Compute integral images if necessary.
   CvPoint intImgOffset, intImgSqrOffset;

   unsigned intImgStride, intImgSqrStride;

   // init integral image params
   CvRect intImgRoi = cvGetImageROI(_intImg);
   CvRect intImgSqrRoi = cvGetImageROI(_intImgSqr);
   intImgOffset.x = intImgRoi.x - roi.x;
   intImgOffset.y = intImgRoi.y - roi.y;
   intImgSqrOffset.x = intImgSqrRoi.x - roi.x;
   intImgSqrOffset.y = intImgSqrRoi.y - roi.y;
   intImgStride = _intImg->widthStep / sizeof(double);
   intImgSqrStride = _intImgSqr->widthStep / sizeof(double);

   // check integral image constraints
   if (intImgRoi.width != roi.width + 1 ||
       intImgRoi.height != roi.height + 1 ||
       intImgSqrRoi.width != roi.width + 1 ||
       intImgSqrRoi.height != roi.height + 1) {
     cerr << "An integral image has the wrong size." << endl;
     assert(false);
   }

   // compute normalization for response image.
   for (int y = roi.y; y < roi.y + roi.height - t->height + 1; y++) {
     //const char* img_row = &CV_IMAGE_ELEM(img, char, y, 0);
     float* resp_row = &CV_IMAGE_ELEM(response, float, y - roi.y, 0);

     for (int x = roi.x; x < roi.x + roi.width - t->width + 1; x++) {
       // compute sum and sumSq for image from integrals
       double imgSum, imgSumSq;

       const double* imgSumPtr = &CV_IMAGE_ELEM(_intImg, double, y + intImgOffset.y, x + intImgOffset.x);
       imgSum = imgSumPtr[0] - imgSumPtr[t->height*intImgStride] - imgSumPtr[t->width] +
         imgSumPtr[t->height*intImgStride + t->width];
       const double* imgSumSqrPtr = &CV_IMAGE_ELEM(_intImgSqr, double,
                                                   y + intImgSqrOffset.y, x + intImgSqrOffset.x);
       imgSumSq = imgSumSqrPtr[0] - imgSumSqrPtr[t->height*intImgSqrStride]
         - imgSumSqrPtr[t->width] + imgSumSqrPtr[t->height*intImgSqrStride + t->width];

       // now compute normalized response
       double imgVar = std::max(imgSumSq - imgSum * imgSum / N, tVar);
       resp_row[x - roi.x] = (resp_row[x - roi.x] - tMean * imgSum) / sqrt(tVar * imgVar);
     }
   }

}
