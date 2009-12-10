/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    train.cpp
** AUTHOR(S):   Stephen Gould <sgould@stanford.edu>
** DESCRIPTION:
**  This file contains the main executable code for training classifiers. It
**  simply loads a list of image filenames (and classes) and passes them to
**  the classifier train() method. Given the directory structure:
**      data/mug/<filestem>.jpg
**      data/stapler/<filestem>.jpg
**      data/other/<filestem>.jpg
**
**  executing with
**      % ./train -c parameters.txt -v data
**
**  will load training files for classes "mug", "stapler", and "other", and
**  save parameters in file "parameters.txt".
**
** CS221 TO DO:
**  You are free to modify anything in this file.
** 
*****************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>

#include <sys/types.h>
#include <dirent.h>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "utils.h"
#include "objects.h"
#include "classifier.h"

using namespace std;

/* Main *********************************************************************/

void usage() {
    cerr << "./train [<options>] <directory>" << endl << endl;
    cerr << "Options:" << endl;
    cerr << "    -lk :: load kmeans" << endl;
    cerr << "    -sk :: save kmeans" << endl;
    cerr << "    -k <num> :: num clusters " << endl;
    cerr << "    -o <num> :: max others " << endl;
    
    cerr << endl << "Classifiers:" << endl;
    cerr << "    -b :: use Naive Bayes" << endl;
    cerr << "    -s :: use SVM" << endl;
    cerr << "    -r :: use Random Forest" << endl;
    cerr << "    -t :: use Boosted Decision Trees" << endl;
    
    cerr << endl << "Other:" << endl;
    cerr << "    -h :: show this message" << endl;
    cerr << "    -v :: verbose" << endl;
    cerr << endl;
}

int main(int argc, char *argv[])
{
    char *configurationFile, *trainingFile, *featuresFile;
    bool bVerbose;
    char **args;
    
    Classifier classifier;

    // set defaults
    configurationFile = NULL;   // set using -c <filename>
    bVerbose = false;           // turn on with -v
    trainingFile = NULL;
    featuresFile = NULL;


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
            featuresFile = *args;
            
            
            
        } else if (!strcmp(*args, "-k")) { 
            argc--; args++;
            sscanf(*args, "%i", &(classifier.num_clusters));
            printf("Set num_clusters to %d \n", classifier.num_clusters);
            
        } else if (!strcmp(*args, "-o")) { 
            argc--; args++;
            int d;
            sscanf(*args, "%i", &d);
            classifier.max_others = d;
            printf("Set max_others to %d \n", d);
            
        } else if (!strcmp(*args, "-lk")) { classifier.kmeans_load = true;
        } else if (!strcmp(*args, "-sk")) { classifier.kmeans_save = true;

        } else if (!strcmp(*args, "-b")) { classifier.bayes.on = true;   
        } else if (!strcmp(*args, "-s")) { classifier.svm.on = true;
        } else if (!strcmp(*args, "-r")) { classifier.rtrees.on = true;
        } else if (!strcmp(*args, "-t")) { classifier.btrees.on = true;
            
            
        } else if (!strcmp(*args, "--save")) { classifier.save_all = true;    
        } else if (!strcmp(*args, "--load")) { classifier.load_all = true;
            
        } else if (!strcmp(*args, "-h")) {
            usage();
            return 0;
        } else if (!strcmp(*args, "-v")) {
            bVerbose = !bVerbose;
        } else {
            cerr << "ERROR: unrecognized option " << *args << endl;
            return -1;
        }
        args++;
    }

    if (argc != 1) { usage(); exit(-1); }
    cout << endl; 
    
    // load the training file list
    TTrainingFileList fileList;
    fileList = getTrainingFiles(*args, ".jpg");
    
    if (!classifier.extract(fileList)) {
        cerr << "ERROR: coult not extract features" << endl;
        exit(-1);
    }
    
    // now train the classifier
    if (!classifier.train()) {
        cerr << "ERROR: could not train classifier" << endl;
        exit(-1);
    }
    // 
    // // save classifier configuration
    // if (configurationFile != NULL) {
    //     if (!classifier.saveState(configurationFile)) {
    //         cerr << "ERROR: could not save classifier configuration" << endl;
    //         exit(-1);
    //     }
    // }

    return 0;
}
