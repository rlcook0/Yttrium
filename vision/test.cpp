/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    test.cpp
** AUTHOR(S):   Stephen Gould <sgould@stanford.edu>
**              Ian Goodfellow <ia3n@stanford.edu>
** DESCRIPTION:
**  This file contains the main executable for running classifiers on
**  test video streams. You should not modify this file.
** EXAMPLE:
**  Executing with command-line
**      % ./test -v -c parameters.txt -g data/easy.xml data/easy/
**
**  will configure the classifier with parameters from file "parameters.txt"
**  and then run on the video "easy.avi" showing ground truth labels from
**  file "easy.xml".
*****************************************************************************/

#include "capture.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <fstream>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "objects.h"
#include "classifier.h"
#include "replay.h"

#define WINDOW_NAME "CS221 Computer Vision Project"

using namespace std;

/* Main *********************************************************************/

void usage() {
    cerr << "./test [<options>] <AVI filename>" << endl << endl;
    cerr << "OPTIONS:" << endl;
    cerr << "    -c <filename>  :: configuration file for classifier" << endl;
    cerr << "    -f <fps>       :: frames per second (0 to pause each frame)" << endl;
    cerr << "    -g <filename>  :: ground truth file" << endl;
    cerr << "    -h             :: show this message" << endl;
    cerr << "    -o <filename>  :: output to file" << endl;
    cerr << "    -v             :: verbose" << endl;
    cerr << "    -x             :: disable display" << endl;
    
    cerr << endl << "More:" << endl;
    cerr << "    -lk :: load kmeans" << endl;
    cerr << "    -sk :: save kmeans" << endl;
    cerr << "    -k <num> :: num clusters " << endl;
    cerr << "    -o <num> :: max others " << endl;
    
    cerr << endl << "Classifiers:" << endl;
    cerr << "    -b :: use Naive Bayes" << endl;
    cerr << "    -s :: use SVM" << endl;
    cerr << "    -r :: use Random Forest" << endl;
    cerr << "    -t :: use Boosted Decision Trees" << endl;
    
    cerr << endl;
}

int main(int argc, char *argv[])
{
    char *configurationFile;
    char *groundTruthFile;
    char *outputFile;
    char *aviFile;
    bool bVerbose;
    bool bShowVideo;
    int fpsDelay;
    char **args;
    
    // set defaults
    configurationFile = NULL;   // set using -c <filename>
    groundTruthFile = NULL;     // set using -g <filename>
    outputFile = NULL;          // set using -o <filename>
    bVerbose = false;           // turn on with -v
    bShowVideo = true;          // turn off with -x
    fpsDelay = 65;              // 65ms delay (~15fps)

    Classifier classifier;

    // check arguments
    args = argv + 1;
    while (argc-- > 2) {
        if (!strcmp(*args, "-c")) {
            argc--; args++;
            if (configurationFile != NULL) {
                usage();
                return -1;
            }
            configurationFile = *args;
        } else if (!strcmp(*args, "-f")) {
            argc--; args++;
            int fps = atoi(*args);
            if (fps <= 0) {
                fpsDelay = 0;
            } else {
                fpsDelay = 1000 / fps;
            }
        } else if (!strcmp(*args, "-g")) {
            argc--; args++;
            if (groundTruthFile != NULL) {
                usage();
                return -1;
            }
            groundTruthFile = *args;
        } else if (!strcmp(*args, "-h") || !strcmp(*args, "--help")) {
            usage();
            return 0;
        } else if (!strcmp(*args, "-o")) {
            argc--; args++;
            if (outputFile != NULL) {
                usage();
                return -1;
            }
            outputFile = *args;
        
            
            
        } else if (!strcmp(*args, "-k")) { 
            argc--; args++;
            sscanf(*args, "%i", &(classifier.num_clusters));
            printf("Set num_clusters to %d \n", classifier.num_clusters);
                
        
        } else if (!strcmp(*args, "-lk")) { classifier.kmeans_load = true;
        } else if (!strcmp(*args, "-sk")) { classifier.kmeans_save = true;

        } else if (!strcmp(*args, "-b")) { classifier.bayes.on = true;   
        } else if (!strcmp(*args, "-s")) { classifier.svm.on = true;
        } else if (!strcmp(*args, "-r")) { classifier.rtrees.on = true;
        } else if (!strcmp(*args, "-t")) { classifier.btrees.on = true;


        } else if (!strcmp(*args, "--save")) { classifier.save_all = true;    
        } else if (!strcmp(*args, "--load")) { classifier.load_all = true;
            
        } else if (!strcmp(*args, "-v")) {
            bVerbose = !bVerbose;
        } else if (!strcmp(*args, "-x")) {
            bShowVideo = !bShowVideo;
        } else {
            cerr << "ERROR: unrecognized option " << *args << endl;
            return -1;
        }
        args++;
    }

    if (argc != 1) {
        usage();
        return -1;
    }

    aviFile = args[0];

    // show settings
    if (bVerbose) {
        cout << "Configuration file: "
             << (configurationFile == NULL ? "(none)" : configurationFile) << endl;
        cout << "Ground truth file:  " 
             << (groundTruthFile == NULL ? "(none)" : groundTruthFile) << endl;
        cout << "Output file:        "
             << (outputFile == NULL ? "(none)" : outputFile) << endl;
        cout << "Test video directory:      " << aviFile << endl;       
    }

    CObjectReplay replayer;
    CvFont font;
    ofstream *outputStream = NULL;

    // configure the classifier
    //if (configurationFile != NULL) {
    if (!classifier.loadState()) {
        cerr << "ERROR: could not configure the classifier" << endl;
        exit(-1);
    }
    //}
    
    // configure the replayer
    if (groundTruthFile != NULL) {
        if (!replayer.configure(groundTruthFile)) {
            cerr << "ERROR: could not configure ground truth replay" << endl;
            exit(1);
        }
    }

    // open the video file
 
    Capture *capture = new Capture(aviFile);
    if (capture == NULL) {
        cerr << "ERROR: could not open AVI file" << endl;
        exit(-1);
    }

    // open the output file
    if (outputFile != NULL) {
        outputStream = new ofstream(outputFile);
        if ((outputStream == NULL) || (!outputStream->good())) {
            cerr << "ERROR: could not open output file" << endl;
            exit(-1);
        }
        *outputStream << "<ObjectList>" << endl;
    }

    // play the video and run classifier
    if (bShowVideo) {
        cvNamedWindow(WINDOW_NAME, CV_WINDOW_AUTOSIZE);
        cvInitFont(&font, CV_FONT_VECTOR0, 0.75, 0.75, 0.0f, 1, CV_AA);
    }

    IplImage *frame = NULL;
    CObjectList classifierObjects;
    CObjectList groundTruthObjects;
    CObjectList::iterator iObj;
    int frameCount = 0;

    while ((frame = capture->queryFrame()) != NULL) {
      int scored = (( ( frameCount- 4) % 10) == 0);

        frameCount += 1;
        if (bVerbose) {
	  cout << "Processing frame " << frameCount << "..." << endl;
	  if (scored)
	    cout << "(scored)" << endl;
        }
        classifierObjects.clear();
        classifier.run(frame, &classifierObjects, scored);
        groundTruthObjects.clear();
        replayer.run(frame, &groundTruthObjects);

        if (outputStream) {
	  *outputStream << "  <frame id=\"" << (frameCount - 1) << "\" scored=\"" << scored << "\">" << endl;
            for (iObj = classifierObjects.begin(); iObj != classifierObjects.end(); iObj++) {
	      *outputStream << "    ";
	      iObj->writeAsXML(*outputStream);
            }
            *outputStream << "  </frame>" << endl;
        }
        
        if (bShowVideo) {
            IplImage *frameCopy = cvCloneImage(frame);
            for (iObj = groundTruthObjects.begin(); iObj != groundTruthObjects.end(); iObj++) {
                iObj->draw(frameCopy, CV_RGB(255, 0, 0), &font);
            }
            for (iObj = classifierObjects.begin(); iObj != classifierObjects.end(); iObj++) {
                iObj->draw(frameCopy, CV_RGB(0, 255, 0), &font);
            }
            cvShowImage(WINDOW_NAME, frameCopy);
            cvReleaseImage(&frameCopy);

            // check for user pressing <esc> and delay for fpsDelay milliseconds
            if (cvWaitKey(fpsDelay) == 27) break;
        }
    }

    // clean up
    if (outputStream) {
        *outputStream << "</ObjectList>" << endl;
        outputStream->close();
        delete outputStream;
    }

    if (bShowVideo) {
        cvDestroyWindow(WINDOW_NAME);
    }



    if (bVerbose)
      cout << "test finished successfully." << endl;


    return 0;
}
