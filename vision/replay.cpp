/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    replay.cpp
** AUTHOR(S):   Stephen Gould <sgould@stanford.edu>
** DESCRIPTION:
**  See replay.h
**
*****************************************************************************/

#include <cassert>
#include <iostream>

#include "replay.h"
#include "CXMLParser.h"

using namespace std;

// CObjectReplay class -------------------------------------------------------

// default constructor
CObjectReplay::CObjectReplay()
{
    nFrameNumber = 0;
}
    
// destructor
CObjectReplay::~CObjectReplay()
{
    // do nothing
}

// configure from file
bool CObjectReplay::configure(const char *filename)
{
    assert(filename != NULL);
    
    CXMLParser xmlParser;
	CXMLElement aCXMLElement;
    int frameIndex;

    nFrameNumber = 0;    
    objectListVector.clear();

    if (!xmlParser.OpenFile(filename)) {
        cerr << "ERROR: could not open configuration file " << filename << endl;
        return false;
    }

    // load object list from configuration file
    if (!xmlParser.NextElement(&aCXMLElement)) {
        cerr << "ERROR: " << xmlParser.GetErrorText() << endl;
        return false;
    }

    if ((aCXMLElement.mType != CXMLElement::TYPE_START) ||
        (aCXMLElement.mValue != "ObjectList")) {
        cerr << "ERROR: Unexpected beginning element found" << endl;
        return false;
    }

    while(!xmlParser.HasFailed()) {
        if (!xmlParser.NextElement(&aCXMLElement)) {
            cerr << "ERROR: " << xmlParser.GetErrorText() << endl;
            return false;
        }

	if (aCXMLElement.mType == CXMLElement::TYPE_END) {
            return true;
	}
	
        // read frame element
        if ((aCXMLElement.mType == CXMLElement::TYPE_START) &&
            (aCXMLElement.mValue == "frame")) {

            frameIndex = atoi(aCXMLElement.mAttributes["id"].c_str());

            while (!xmlParser.HasFailed()) {
                if (!xmlParser.NextElement(&aCXMLElement)) {
                    cerr << "ERROR: " << xmlParser.GetErrorText() << endl;
                    return false;
                }
                if ((aCXMLElement.mValue == "frame") &&
                    (aCXMLElement.mType == CXMLElement::TYPE_END)) {
                        break;
                }

                if ((aCXMLElement.mValue == "object") &&
                    (aCXMLElement.mType != CXMLElement::TYPE_END)) {

                    // create the object
                    CObject obj;
                    obj.rect = cvRect(
			atoi(aCXMLElement.mAttributes["x"].c_str()),
                        atoi(aCXMLElement.mAttributes["y"].c_str()),
                        atoi(aCXMLElement.mAttributes["w"].c_str()),
                        atoi(aCXMLElement.mAttributes["h"].c_str())
					);

                    if( aCXMLElement.mAttributes.find("label") != aCXMLElement.mAttributes.end() ) {
			obj.label = aCXMLElement.mAttributes["label"];
		    }

                    // add the object to the correct frame
                    while ((int)objectListVector.size() <= frameIndex) {
                        CObjectList emptyList;
                        objectListVector.push_back(emptyList);
                    }

                    objectListVector[frameIndex].push_back(obj);
                }
            }
        }
    }
        
    return false;
}

// run
// Runs the replay over the given frame and returns a list of
// objects found (and their location).
bool CObjectReplay::run(const IplImage *frame, CObjectList *objects)
{
    assert((frame != NULL) && (objects != NULL));
    
    if ((int)objectListVector.size() > nFrameNumber) {
        objects->insert(objects->begin(),
	    objectListVector[nFrameNumber].begin(),
            objectListVector[nFrameNumber].end());
    }

    nFrameNumber += 1;
    return true;
}
        

