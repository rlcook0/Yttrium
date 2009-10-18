/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    objects.h
** AUTHOR(S):   Stephen Gould <sgould@stanford.edu>
** DESCRIPTION:
**  This file defines the CObject and CObjectList classes.
**
*****************************************************************************/

#pragma once

#include <iostream>
#include <vector>

#include "cv.h"
#include "cxcore.h"

/* CObject class -------------------------------------------------------------
 * This class holds annotation for a single object. It includes the object's
 * bounding box and label. You can extend the class to include other data
 * such as a Kalman filter for tracking objects from one frame to the next.
 */
class CObject {
public:
    CvRect rect;            // object's bounding box
    std::string label;      // object's class

public:
    // constructors
    CObject();
    CObject(const CObject&);
    CObject(const CvRect&, const std::string&);
    
    // destructor
    virtual ~CObject();
    
    // helper functions
    void writeAsXML(std::ostream&);
    void draw(IplImage *, CvScalar, CvFont *);
    CvRect intersect(const CObject&) const;
    int overlap(const CObject&) const;  
    int area() const { return rect.width * rect.height; }

    // operators
    CObject& operator=(const CObject&);
};

/* CObjectList class ---------------------------------------------------------
 * This class contains a list of objects for a given video frame.
 */
 
typedef std::vector<CObject> CObjectList;

