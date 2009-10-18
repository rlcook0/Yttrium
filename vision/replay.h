/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    replay.h
** AUTHOR(S):   Stephen Gould <sgould@stanford.edu>
** DESCRIPTION:
**  Replays ground truth (or previously recorded classifications) from an
**  XML file. Do not modify this file.
*****************************************************************************/

#pragma once

#include "cv.h"
#include "cxcore.h"

#include "objects.h"

/* CObjectReplay class --------------------------------------------------------
 */
class CObjectReplay {
private:
    int nFrameNumber;
    std::vector<CObjectList> objectListVector;
    
public:
    // constructors
    CObjectReplay();
    
    // destructor
    ~CObjectReplay();

    // configure from file
    bool configure(const char *);

    // replay object for a single frame
    bool run(const IplImage *, CObjectList *);
        
private:
};

