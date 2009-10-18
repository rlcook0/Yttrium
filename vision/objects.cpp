/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    objects.cpp
** AUTHOR(S):   Stephen Gould <sgould@stanford.edu>
** DESCRIPTION:
**  See objects.h
**
*****************************************************************************/

#include <cassert>

#include "objects.h"

using namespace std;

// CObject class -------------------------------------------------------------

// default constructor
CObject::CObject()
{
    // do nothing
}

// copy constructor
CObject::CObject(const CObject& obj)
{
    rect = obj.rect;
    label = obj.label;
}

// full constructor
CObject::CObject(const CvRect& r, const string &s)
{
    rect = r;
    label = s;
}

// destructor
CObject::~CObject()
{
    // do nothing
}

// writeAsXML
// Writes the object out as XML formatted text.
void CObject::writeAsXML(ostream& os)
{
    os  << "<object x=\"" << rect.x << "\""
        << " y=\"" << rect.y << "\""
        << " w=\"" << rect.width << "\""
        << " h=\"" << rect.height << "\""
        << " label=\"" << label.c_str() << "\""
        << " />" << endl;
}

// draw
// Draws the object with label on an IplImage
void CObject::draw(IplImage *image, CvScalar color, CvFont *font)
{
    assert(image != NULL);
    
    // draw the bounding box
    cvRectangle(image, cvPoint(rect.x, rect.y),
        cvPoint(rect.x + rect.width, rect.y + rect.height),
        color, 2);
    
    // draw the label text
    if (font != NULL) {
        cvPutText(image, label.c_str(), cvPoint(rect.x + 3, rect.y - 3),
            font, color);
    }
}

// intersect
// Computes the intersection between any two objects.
CvRect CObject::intersect(const CObject& obj) const
{
    CvRect intersection;
    
    // find overlapping region
    intersection.x = (rect.x < obj.rect.x) ? obj.rect.x : rect.x;
    intersection.y = (rect.y < obj.rect.y) ? obj.rect.y : rect.y;
    intersection.width = (rect.x + rect.width < obj.rect.x + obj.rect.width) ?
        rect.x + rect.width : obj.rect.x + obj.rect.width;
    intersection.width -= intersection.x;
    intersection.height = (rect.y + rect.height < obj.rect.y + obj.rect.height) ?
        rect.y + rect.height : obj.rect.y + obj.rect.height;
    intersection.height -= intersection.y;    
    
    // check for non-overlapping regions
    if ((intersection.width <= 0) || (intersection.height <= 0)) {
        intersection = cvRect(0, 0, 0, 0);
    }
    
    return intersection;
}

// overlap
// Computes the number of pixels that overlap between two objects
// of the same class.
int CObject::overlap(const CObject& obj) const
{
    CvRect overlappingRegion;
    
    // if the labels aren't the same then, by definition, there
    // can be no overlap
    if (obj.label != label) {
        return 0;
    }
    
    overlappingRegion = this->intersect(obj);

    // compute size of overlapping region
    return (overlappingRegion.width * overlappingRegion.height);
}

// assignment operator
CObject& CObject::operator=(const CObject& obj)
{
    rect = obj.rect;
    label = obj.label;
    
    return *this;
}

