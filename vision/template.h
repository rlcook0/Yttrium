/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2009, Stanford University
**
** FILENAME:    template.h
** AUTHOR(S):   Ian Goodfellow <ia3n@stanford.edu>
** DESCRIPTION:
**  Computes the response image (see the handout for details).
**
*****************************************************************************/

#pragma once

#include "cv.h"
#include "cxcore.h"

class TemplateMatcher
{
 public:

  TemplateMatcher();
  ~TemplateMatcher();

  /** Sets the video frame (or still image) to be used for subsequent
      calls to makeResponseImage. You should only call this once per
      frame, and then call makeResponseImage repeatedly with different
      templates.

      The TemplateMatcher stores a pointer to the image. It will not
      modify or delete it. The caller must also not modify or delete the image
      until after all calls to makeResponseImage().
  */
  void loadFrame(const IplImage * img);

  /** Matches a template across the image most recently loaded using
      loadFrame. Caller is responsible for allocating IplImage *
      response with the correct size */
  void makeResponseImage(const IplImage * t, IplImage* response) const;

 protected:
  //not owned by this class
  const IplImage * _img;
  //owned by this class
  IplImage * _intImg;
  IplImage * _intImgSqr;

};
