/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME: classifier.cpp
** AUTHOR(S): Stephen Gould <sgould@stanford.edu>
** Ian Goodfellow <ia3n@stanford.edu>
** DESCRIPTION:
** See classifier.h
**
*****************************************************************************/
 
#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>
 
#include "highgui.h"

#include "surf.h"
#include "surflib.h"

#include "cv.h"
#include "cxcore.h"
 
#include "classifier.h"
#include "logreg.h"

// Classifier class ---------------------------------------------------------
 
// default constructor
Classifier::Classifier()
{
    // initalize the random number generator (for sample code)
    rng = cvRNG(-1);
 
    //centers = NULL;
 
    //_features.load("dict.xml");
    //_regressor = new LogReg(_features.numFeatures(), double(0.001));
    
}
    
// destructor
Classifier::~Classifier()
{
    //delete _regressor;
    // CS221 TO DO: free any memory allocated by the object
}
 
// loadState
// Configure the classifier from the given file.
bool Classifier::loadState(const char *filename)
{
    assert(filename != NULL);
    
    //ifstream infile;
    //infile.open(filename);
    
    /*double val;
    int i = 0;
    while(!infile.fail() && !infile.eof()) {
        infile >> val;
        if (infile.fail() || infile.eof()) return true;
        
        _regressor->set(i++, val);
    }*/
    
    //CvFileStorage* fs = cvOpenFileStorage( "bayes.yml", 0, CV\_STORAGE\_WRITE );
    
    bayes.load("bayes.dat");
    centers = (CvMat *)cvLoad("centers.dat");
    
    // CvMat *newC = cvCreateMat(centers->rows, centers->cols, CV_32FC1);
    // cvCopy(newC, centers);
    // cvReleaseMat(&centers);
    // centers = newC;
    
    //fprintf("%d %d", centers.
    
    cout << "centers: "<<centers << endl;
    
    loadSURFFile(filename, &surfFeatures);
    
    int total = 0;
    for (map<string, vector<Ipoint> >::const_iterator it = surfFeatures.begin(); it != surfFeatures.end(); ++it) {
        total += it->second.size();
        printf("%s %d\n", it->first.c_str(), total);
        surfThresh.push_back(pair<string, int>(it->first, total));
        surfTotal[it->first] = (int)(it->second.size());
    }
    surfTotalIpoints = total;
    
    CvMat *desc = cvCreateMat(total, 64, CV_32FC1);

    int where = 0;
    for (map<string, vector<Ipoint> >::const_iterator it = surfFeatures.begin(); it != surfFeatures.end(); ++it) {
        const vector<Ipoint> *pts = &(it->second);
        for (unsigned i = 0; i < it->second.size(); ++i) {
            const Ipoint *ipt = &((*pts)[i]);
            const float *vals = ipt->descriptor;
            for (int j = 0; j < 64; ++j) {
                //printf(" (%d %d %f)", where, j, vals[j]);
                cvSetReal2D(desc, where, j, vals[j]);
            }
            ++where;
        }
    }
    
    printf("how many? %d\n", total);
    
    surfFT = cvCreateKDTree(desc);
    
    printf("made surf tree!\n");
    
    /*for (int i = 0; i < centers->rows; ++i) {
        for (int j = 0; j < centers->cols; ++j) {
            printf("%f ", cvGetReal2D(centers, i, j));
        }
        printf("\n");
    }*/
    
    //centersFT = cvCreateKDTree(centers);
    
    //printf("made centers tree!\n");
    cout << "centers: " << centers << endl;
    
    return true;
}
 
// saveState
// Writes classifier configuration to the given file
bool Classifier::saveState(const char *filename)
{
    assert(filename != NULL);
    
    /*ofstream outfile;
    outfile.open(filename); //<= because of bias.
    for (unsigned i = 0; i <= _features.numFeatures(); i++) {
        outfile << _regressor->get(i) << ' ';
    }
    outfile.close();*/
    
    saveSURFFile(filename, &surfFeatures);
    
    return true;
}

// bool Classifier::loadTrainingFile(const char *filename, std::vector<Trainer> *trainers)
// {
    // if (filename == NULL) return false;
    
    // ifstream infile;
    // infile.open(filename);
    
    // int vars, count;
    
    // infile >> count;
    // if (infile.fail() || infile.eof()) return false;
    
    // infile >> vars;
    // if (infile.fail() || infile.eof()) return false;    
    
    // for (int i = 0; i < count; i++) 
    // {
    
	    // infile.ignore(1);
	    
        // Trainer t;
        // infile >> t.truth;
     	
     	// t.values = new double[vars + 1];
        
        // for (int j = 0; j <= vars; j ++) 
        // {
            // infile >> t.values[j];
        // }
        // trainers->push_back(t);
    // }
    
    // cout << "Loaded training feature values from: " << filename << endl;
    
    // return true;
// }

// saveState
// Writes classifier TrainingFile to the given file
// bool Classifier::saveTrainingFile(const char *filename, std::vector<Trainer> *trainers)
// {
    // if (filename == NULL) return false;

    // ofstream outfile;
    // outfile.open(filename); //<= because of bias.

    // outfile << trainers->size() << ' ' << _features.numFeatures() << endl;

    // for (unsigned i = 0; i < trainers->size(); i++) 
    // {
        // outfile << (*trainers)[i].truth << ' ';
        // for (unsigned j = 0; j <= _features.numFeatures(); j++) 
        // {
            // outfile << (*trainers)[i].values[j] << ' ';
        // }
        // outfile << endl;
    // }
    // outfile.close();

    // cout << "Wrote training feature values from: " << filename << endl;

    // return true;
// }
 
// saveState
// Writes classifier TrainingFile to the given file
bool Classifier::saveSURFFile(const char *filename, map<string, vector<Ipoint> > *des)
{
    if (filename == NULL) return false;
    
    ofstream outfile;
    outfile.open(filename);
    
    outfile << des->size() << endl;
    
    for(map<string, vector<Ipoint> >::iterator it = des->begin(); it != des->end(); ++it)
    {
        outfile << it->second.size() << " " << it->first << endl; 
        for (unsigned j = 0; j < it->second.size(); j++)
        {
            Ipoint i = (it->second)[j];
            outfile << i.x << "," << i.y << " " << i.scale << " " << i.orientation << " " << i.laplacian << " ";
            for (unsigned k = 0; k < 64; k++)
                outfile << i.descriptor[k] << " ";
            outfile << endl;
        }
        outfile << endl;
    }
    outfile.close();
    
    cout << "Wrote SURF descriptor values to: " << filename << endl;

    return true;
}


bool Classifier::loadSURFFile(const char *filename, map<string, vector<Ipoint> > *des)
{
    if (filename == NULL) return false;
    
    ifstream infile;
    infile.open(filename);
    
    int count, ipoints;
    
    infile >> count;
    if (infile.fail() || infile.eof()) return false;    
    infile.ignore(1);
    
    for (int c = 0; c < count; c++) 
    {
	    
        string name;
        infile >> ipoints;
        infile.ignore(1);        
        infile >> name; 
	    infile.ignore(1);
        
        printf("reading %s: %d\n", name.c_str(), ipoints);
        
        vector<Ipoint> v;
        for (int j = 0; j < ipoints; j++) 
        {
            Ipoint i;
            char coma;
            infile >> i.x >> coma >> i.y >> i.scale >> i.orientation >> i.laplacian;
            for (int k = 0; k < 64; k++) 
            {   infile >> i.descriptor[k]; }
            v.push_back(i);
            infile.ignore(1);
        }
        (*des)[name] = v;
        infile.ignore(1);
    }
    infile.close();
    
    cout << "Loaded SURF descriptor values from: " << filename << endl;
    cout << "centers: " << centers << endl;
    return true;
}

string Classifier::indexToClass(int index) {
    for (unsigned i = 0; i < surfThresh.size(); ++i) {
        if (index < surfThresh[i].second) {
            return surfThresh[i].first;
        }
    }
    
    return "ERROR";
}

int Classifier::indexToClassInt(int index) {
    string klass = indexToClass(index);
    return stringToClassInt(klass);
}

int Classifier::stringToClassInt(string type) {
    if (type == "other")
        return kOther;
    if (type == "mug")
        return kMug;
    if (type == "keyboard")
        return kKeyboard;
    if (type == "clock")
        return kClock;
    if (type == "stapler")
        return kStapler;
    if (type == "scissors")
        return kScissors;
    
    return -1;
}

string Classifier::classIntToString(int type) {
    if (type == kOther)
        return "other";
    if (type == kMug)
        return "mug";
    if (type == kKeyboard)
        return "keyboard";
    if (type == kClock)
        return "clock";
    if (type == kStapler)
        return "stapler";
    if (type == kScissors)
        return "scissors";
    
    return "error";
}

// TESTING ONLY
bool Classifier::showRect(const IplImage *image, CObject *rect, const vector<Ipoint> *pts = NULL, const CvMat* indicies = NULL, const CvMat *dist = NULL) {
    //char *WINDOW_NAME = "test";
    CvFont font;

    cvNamedWindow("test", CV_WINDOW_AUTOSIZE);
    cvInitFont(&font, CV_FONT_VECTOR0, 0.75, 0.75, 0.0f, 1, CV_AA);
    
    IplImage *frameCopy = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
    cvConvertImage(image, frameCopy);
    
    if (pts != NULL) {
        for (int i = 0; i < (int)pts->size(); ++i) {
            CvScalar color = CV_RGB(255, 0, 0);
            if (indicies != NULL && dist != NULL) {
                
                int index = cvGetReal2D(indicies, i, 0);
                string type = indexToClass(index);
                
                map<string, double> votes;
                double totalVotes = 0;
                for (int col = 0; col < indicies->cols; ++col) {
                    int index = cvGetReal2D(indicies, i, col);
                    if (index >= 0) {
                        string type = indexToClass(index);
                        double d = cvGetReal2D(dist, i, col);
                        //printf("%d %f %s\n", index, dist, type.c_str());
                        
                        if (d < 0.5) {
                            votes[type] += 1;
                            totalVotes += 1;
                        }
                    }
                }
                //printf("figured out votes\n");
                
                string maxLabel = "ERROR";
                double maxVotes = -1;
                for(map<string, double>::const_iterator it = votes.begin(); it != votes.end(); ++it) {
                    //cout << it->first << ": " << (double)it->second / (double)totalVotes << "%";
                    //printf("\n");
                    
                    if (it->second > maxVotes) {
                        maxLabel = it->first;
                        maxVotes = it->second;
                    }
                }
                
                if (maxLabel == "mug") // white
                    color = CV_RGB(255, 255, 255);
                else if (maxLabel == "other") // orange
                    color = CV_RGB(255, 100, 100);
                else if (maxLabel == "scissors") // blue
                    color = CV_RGB(100, 100, 255);
                else if (maxLabel == "stapler") // green
                    color = CV_RGB(0, 255, 0);
                else if (maxLabel == "clock") // red
                    color = CV_RGB(255, 0, 0);
                else if (maxLabel == "keyboard") // cyan
                    color = CV_RGB(0, 255, 255);
            }
                
            cvRectangle(frameCopy, cvPoint((*pts)[i].x, (*pts)[i].y), 
                cvPoint((*pts)[i].x + 3, (*pts)[i].y + 3), color);
        }
    }
    
    rect->draw(frameCopy, CV_RGB(255, 0, 255), &font);
    
    
    cvShowImage("test", frameCopy);
    cvReleaseImage(&frameCopy);

    return cvWaitKey(100) != -1;
}

int frameNo = 0;
// run
// Runs the classifier over the given frame and returns a list of
// objects found (and their location).
// you only need to populate "objects" if "scored" is true. if you do
// populate "objects" when "scored" is false, it will not affect your 
// score but it may be helpful to you for debugging (to visualize your
// results on more frames)
bool Classifier::run(const IplImage *frame, CObjectList *objects, bool scored)
{
  if (!scored)// || ++frameNo < 100)
    return true;
    
    cout << "centers: " << centers << endl;

  assert((frame != NULL) && (objects != NULL));
  
  // Convert to grayscale.
  IplImage *gray = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
  cvCvtColor(frame, gray, CV_BGR2GRAY);
  
  // Resize by half first, as per the handout.
  double scale = 2.0;
  IplImage *dst = cvCreateImage(cvSize(gray->width / scale, gray->height / scale), gray->depth, gray->nChannels);
  cvResize(gray, dst);
  
  // Do six further resizes, evaluating each time.
  int numLayers = 7;
  //int numLayers = 1;
  
  //TemplateMatcher tm;
  
  //int absBestX = INT_MIN, absBestY = INT_MIN;
  //double absBestScore = INT_MIN, absBestScale = scale;
  
  while(numLayers-- > 0) {
    //bool skipped = false;
    //tm.loadFrame(dst);
   
   /*
    // Store feature maps.
    std::vector<IplImage *> images;
    int maxWidth = INT_MAX, maxHeight = INT_MAX;
    for (unsigned featureNum = 0; featureNum < _features.numFeatures(); featureNum++) {
        FeatureDefinition *fd = _features.getFeature(featureNum);
        CvRect r = fd->getValidRect();
        
        CvSize newSize = cvSize(dst->width - fd->getTemplateWidth() + 1, dst->height - fd->getTemplateHeight() + 1);
        IplImage *featureMap = cvCreateImage(newSize, IPL_DEPTH_32F, 1);
        tm.makeResponseImage(fd->getTemplate(), featureMap);
        images.push_back(featureMap);
        
        //cout << featureMap->width << " ";
        
        // Maintain the furthest we can slide the 32x32 window.
        if (featureMap->width < maxWidth) maxWidth = featureMap->width;
        if (featureMap->height < maxHeight) maxHeight = featureMap->height;
    }
    */
    // Compute sum of theta(i) x(i).
    //int bestX = INT_MIN, bestY = INT_MIN;
    //double bestScore = INT_MIN;
    
    std::vector<Ipoint> pts;
    surfDetDes(dst, pts, false);//, OCTAVES, INTERVALS, 1, 0.0004f);
    
    printf("found surf points\n");
    CvMat* descriptors = cvCreateMat(pts.size(), 64, CV_32FC1);
    for (int i = 0; i < (int)pts.size(); ++i) {
        Ipoint* ipt = &(pts[i]);
        for (int j = 0; j < 64; ++j) {
            //printf(" (%d %d %f)", i, j, ipt->descriptor[j]);
            cvSetReal2D(descriptors, i, j, ipt->descriptor[j]);
        }
    }
    
    int k = 50;
    CvMat *indicies = cvCreateMat(pts.size(), k, CV_32SC1), *distances = cvCreateMat(pts.size(), k, CV_64FC1);
    cvFindFeatures(surfFT, descriptors, indicies, distances, k);
// original thresh is 0.0004f

    printf("finding shit\n");
    vector<int> cluster(pts.size());
    for (int i = 0; i < (int)pts.size(); ++i) {
        float min_distance = INT_MAX;
        int min_index = -1;
        for (int c = 0; c < 500; ++c) {
            float dist = 0;
            for (int j = 0; j < 64; ++j) {
                dist += pow(cvGetReal2D(descriptors, i, j) - cvGetReal2D(centers, c, j), 2);
            }
            
            if (dist < min_distance) {
                min_index = c;
                min_distance = dist;
            }
        }
        
        cluster[i] = min_index;
    }
                       
    int maxWidth = dst->width;
    int maxHeight = dst->height;
    for (int x = 0; x < maxWidth - 32*scale; x += 8) {
        for (int y = 0; y < maxHeight - 32*scale; y += 8) {

            //printf("setting roi\n");
            CvRect r = cvRect(x, y, 32 * scale, 32 * scale);
            //cvSetImageROI(dst, r);
            
            //printf("doing surf\n");
            std::vector<int> newpts;
            for (int i = 0; i < (int)pts.size(); ++i) {
                if (pts[i].x >= x && pts[i].x < x + 32*scale && pts[i].y >= y && pts[i].y < y + 32*scale)
                    newpts.push_back(i);
            }
            
            if (newpts.size() < 5) continue;
            
            //printf("found features\n");
            //printf("%d features\n", (int)pts.size());
            // Figure out votes
            //map<string, double> votes;
            //double totalVotes = 0;
            /*for (int row = 0; row < (int)newpts.size(); ++row) {
                for (int col = 0; col < indicies->cols; ++col) {
                    int index = cvGetReal2D(indicies, newpts[row], col);
                    if (index >= 0) {
                        string type = indexToClass(index);
                        double dist = cvGetReal2D(distances, newpts[row], col);
                        //printf("%d %f %s\n", index, dist, type.c_str());
                        
                        if (dist < 0.5) {
                            votes[type] += (0.5 - dist) / 0.5;
                            totalVotes += (0.5 - dist) / 0.5;
                        }
                    }
                }
                //printf("\n");
            }*/
            
            CvMat *query = cvCreateMat(1, 500, CV_32FC1);
            for (int row = 0; row < (int)newpts.size(); ++row) {
                int idx = newpts[row];
                int cluster_idx = cluster[idx];

                int oldVal = cvGetReal2D(query, 0, cluster_idx);
                cvSetReal2D(query, 0, cluster_idx, oldVal+1);
            }
            
            int klass = bayes.predict(query);
            printf("it's a %d %s!\n", klass, classIntToString(klass).c_str());
            
            /*
            
            printf("figured out votes\n");
            
            string maxLabel = "NONE";
            double maxVotes = -1;
            for(map<string, double>::const_iterator it = votes.begin(); it != votes.end(); ++it) {
                int total = surfTotal[it->first];
                double proportion = (it->second / (double)total);
                cout << it->first << ": " << proportion;
                printf("\n");
                
                if (proportion > maxVotes) {// && (double)it->second / (double)totalVotes > 0.1 && it->first != "other") {
                    maxLabel = it->first;
                    maxVotes = it->second;
                }
            }*/
            
            cvResetImageROI(dst);
            
            CObject o;
            o.rect = r;
            o.label = classIntToString(klass);
            
            showRect(dst, &o, &pts, indicies, distances);
        }
    }
    
    /*
    // TODO: figure out.
    maxWidth = dst->width;
    maxHeight = dst->height;
    
    for (int x = 0; x < maxWidth; x += 8) {
        for (int y = 0; y < maxHeight; y += 8) {
        
            double score = 0;
            bool bad = false;
            for (unsigned featureNum = 0; featureNum < _features.numFeatures(); featureNum++) {
                FeatureDefinition *fd = _features.getFeature(featureNum);
                CvRect r = fd->getValidRect();
                r.x += x;
                r.y += y;
                IplImage *featureMap = images[featureNum];
                
                if (r.x + r.width > featureMap->width || r.y + r.height > featureMap->height) {
                    bad = true;
                    break;
                }
                
                double value = this->maxpool(featureMap, r); 
                score += value * _regressor->get(featureNum);
                
                // if (!bad && !skipped) {
                    // CObject obj;
                    // obj.rect = r;
                    // char buffer[50];
                    // sprintf(buffer, "%f", value * _regressor->get(featureNum));
                    // obj.label = buffer;
                    // bool did_skip = showRect(dst, &obj);
                    // if (did_skip) skipped = true;
                // }
            }
            
            // Add bias.
            score += _regressor->get(_features.numFeatures());
            
            // if (!bad && !skipped) {
                // CObject obj;
                // obj.rect = cvRect(x * scale, y * scale, 32 * scale, 32 * scale);
                // char buffer[50];
                // sprintf(buffer, "%f", score);
                // obj.label = buffer;
                // bool did_skip = showRect(gray, &obj);
                // if (did_skip) skipped = true;
            // }
            
            //cout << "x: " << x << " y: " << y << " score: " << score << endl;
            if (!bad && score > bestScore) {
                bestScore = score;
                bestX = x;
                bestY = y;
            }
            
        }
        
    }
    */

    // Free old images.
    /*for (unsigned imageNum = 0; imageNum < images.size(); imageNum ++) {
        cvReleaseImage(&images[imageNum]);
    }
    
    if (bestScore > absBestScore) {
        absBestScore = bestScore;
        absBestX = bestX;
        absBestY = bestY;
        absBestScale = scale;
    }
    */
    // Do new resize.
    scale *= 1.2;
    // IplImage *old = dst;
    
    // CvSize newSize = cvSize(gray->width / scale, gray->height / scale);
    // dst = cvCreateImage(newSize, gray->depth, gray->nChannels);
    // cvResize(gray, dst);
    
    // cvReleaseImage(&old);
  }
  
  // Logistic regression.
    /*double logScore = 1.0 / (1.0 + exp(-1.0 * absBestScore));

    cout << "score: " << absBestScore << " logScore: " << logScore << endl;

    // If logScore > 0.5, we have a match!
    if (logScore > 0.5) {
        CObject obj;
        obj.rect = cvRect(absBestX * absBestScale, absBestY * absBestScale, 32 * absBestScale, 32 * absBestScale);
        obj.label = "mug";
        objects->push_back(obj);
    }*/
  
  cvReleaseImage(&gray);
  return true;
}

// double Classifier::maxpool(IplImage *r, const CvRect &pool)
// {
    // float max = -1.0;
    
    // for (int x = pool.x; x < pool.x + pool.width; x++) {
        // for (int y = pool.y; y < pool.y + pool.height; y++) {
            // float val = CV_IMAGE_ELEM(r, float, y, x);
            // if ( val > max)
                // max = val;
        // }
    // }
    
    // return max;
// }
 
// double *Classifier::feature_values(IplImage *dst, TemplateMatcher *tm)
// {
    // tm->loadFrame(dst);
    // FeatureDefinition *fd = NULL;
    
    // double *values = new double[_features.numFeatures()];
    // for (unsigned i = 0; i < _features.numFeatures(); i++) {
        
        // fd = _features.getFeature(i);
        
        // CvSize newSize = cvSize(dst->width - fd->getTemplateWidth() + 1, dst->height - fd->getTemplateHeight() + 1);
        // IplImage *response = cvCreateImage(newSize, IPL_DEPTH_32F, 1);
        
        // tm->makeResponseImage(fd->getTemplate(), response);
        // CvRect valid = fd->getValidRect();
        // values[i] = this->maxpool(response, valid);

        // cvReleaseImage(&response);
        
    // }
    // return values; // REMEMBER TO FREE!
// }
 
// train
// Trains the classifier to recognize the objects given in the
// training file list.
bool Classifier::train(TTrainingFileList& fileList, const char *trainingFile)
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
    
    //std::vector<Trainer> values;
    //TemplateMatcher tm;
 
//    int maxMugs = INT_MAX;
    int maxOther = 200;
    int maxImages = INT_MAX;
    vector<Ipoint> all;
    
    
    // (class, <ipt, ipt...>)
    vector<pair<string, vector<Ipoint> > > allImages;
 
//    CvMemStorage* storage = cvCreateMemStorage(0);
    
    //bool extractVector = !this->loadTrainingFile(trainingFile, &values);
    
    printf("starting\n");
	    cout << "Processing images..." << endl;
	    smallImage = cvCreateImage(cvSize(32, 32), IPL_DEPTH_8U, 1);
    
        int minfiles = min(maxImages, (int)fileList.files.size());
        for (int i = 0; i < minfiles; i++) {
            // show progress
            if (fileList.files[i].label == "other" && --maxOther < 0) continue;
            if (i % 10 == 0) showProgress(i, minfiles);
            
            //printf("loading image\n");
            // load the image
            image = cvLoadImage(fileList.files[i].filename.c_str(), 0);
            if (image == NULL) {
                cerr << "ERROR: could not load image " << fileList.files[i].filename.c_str() << endl;
                continue;
            }
            
            //printf("surf\n");
            // SURF.
            std::vector<Ipoint> pts;
            surfDetDes(image, pts, false);
            
            //printf("was it here\n");
            
            //printf("done surf\n");
            for (unsigned j = 0; j < pts.size(); ++j) {
                surfFeatures[fileList.files[i].label].push_back(pts[j]);
                //all.push_back(pts[j]);
            }
            
            allImages.push_back(pair<string, vector<Ipoint> >(fileList.files[i].label, pts));
            
            //printf("or was it here\n");
            
            // resize to 32 x 32
            //cvResize(image, smallImage);
        
            //Trainer t;
            //t.values = this->feature_values(smallImage, &tm);
            //t.truth = (fileList.files[i].label == "mug");
        
            //values.push_back(t);
        
            // free memory
            cvReleaseImage(&image);
        }
        
        // free memory
        cvReleaseImage(&smallImage);
        cout << endl;

    int total = 0;
    for (map<string, vector<Ipoint> >::const_iterator it = surfFeatures.begin(); it != surfFeatures.end(); ++it) {
        total += it->second.size();
        printf("%s %d\n", it->first.c_str(), (int)it->second.size());
        surfThresh.push_back(pair<string, int>(it->first, total));
    } 
    
    printf("setting descriptor\n");
    CvMat *all_desc = cvCreateMat(total, 64, CV_32FC1);
    for (int i = 0; i < (int)allImages.size(); ++i) {
        for (int iptNo = 0; iptNo < (int)allImages[i].second.size(); ++iptNo) {
            //printf("%d %d\n", i, iptNo);
            Ipoint *pt = &(allImages[i].second[iptNo]);
            for (int j = 0; j < 64; ++j) {
                cvSetReal2D(all_desc, i, j, pt->descriptor[j]);
            }
        }
    }
    
    printf("kmeans\n");
    int numClusters = 2000;
    CvMat *clusters = cvCreateMat(total, 1, CV_32SC1);
    centers = cvCreateMat(numClusters, 64, CV_32FC1);
    cvKMeans2(all_desc, // samples
        numClusters, // clusters
        clusters, // labels
        cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 0.00001), // termcrit
        1, // attempts
        &rng, //rng
        0, // flags
        centers, // centers
        NULL); // compactness*/
    //int cvKMeans2(const CvArr* samples, int nclusters, CvArr* labels, CvTermCriteria termcrit, int attempts=1, CvRNG* rng=0, int flags=0, CvArr* centers=0, double* compactness=0)�
    
    printf("done kmeans\n");
    
    CvMat *newDesc = cvCreateMat(allImages.size(), numClusters, CV_32FC1);
    cvSet(newDesc, cvRealScalar(0));
    int where = 0;
    for(int i = 0; i < (int)allImages.size(); ++i) {
        for (int iptNo = 0; iptNo < (int)allImages[i].second.size(); ++iptNo) {
            int cluster = cvGetReal1D(clusters, where);
            
            //printf("(%d %d)\n", i, cluster);
            int oldVal = cvGetReal2D(newDesc, i, cluster);
            cvSetReal2D(newDesc, i, cluster, oldVal+1);
            ++where;
        }
    }
    
    printf("filling bayes\n");
    //CvMat *train_data = cvCreateMat(all.size(), 1, CV_32FC1);
    CvMat *responses = cvCreateMat(allImages.size(), 1, CV_32SC1);
    for (int i = 0; i < (int)allImages.size(); ++i) {
        int klass = stringToClassInt(allImages[i].first);
        //printf("%d class is %d %s\n", i, klass, classIntToString(klass).c_str());
        //int cluster = cvGetReal1D(clusters, i);
        
        //cvSetReal1D(train_data, i, cluster);
        cvSetReal1D(responses, i, klass);
    }
    
    printf("training bayes\n");
    for (int i = 0; i < 1; ++i) {
        bayes.train(newDesc, responses, 0, 0, i > 0);
    }
    
    printf("training svm\n");
    CvSVM svm;
    svm.train(newDesc, responses);
    
    printf("training knn\n");
    CvKNearest knn(newDesc, responses);
    
    //printf("training dtree\n");
    //CvDTree dtree;
    //dtree.train(newDesc, CV_ROW_SAMPLE, responses);
    
    printf("testing bayes\n");
    for (int img = 0; img < (int)allImages.size(); ++img) {
        vector<Ipoint> pts = allImages[img].second;
        vector<int> cluster(pts.size());
        for (int i = 0; i < (int)pts.size(); ++i) {
            float min_distance = INT_MAX;
            int min_index = -1;
            for (int c = 0; c < numClusters; ++c) {
                float dist = 0;
                for (int j = 0; j < 64; ++j) {
                    dist += pow(pts[i].descriptor[j] - cvGetReal2D(centers, c, j), 2);
                }
                
                if (dist < min_distance) {
                    min_index = c;
                    min_distance = dist;
                }
            }
            
            cluster[i] = min_index;
        }
        
        CvMat *query = cvCreateMat(1, numClusters, CV_32FC1);
        cvSet(query, cvScalar(0));
        for (int row = 0; row < (int)pts.size(); ++row) {
            int cluster_idx = cluster[row];

            int oldVal = cvGetReal2D(query, 0, cluster_idx);
            cvSetReal2D(query, 0, cluster_idx, oldVal+1);
        }
        
        int klass = bayes.predict(query);
        int klass2 = svm.predict(query);
        int klass3 = knn.find_nearest(query, 5);
        printf("%s is a %s (bayes)", allImages[img].first.c_str(), classIntToString(klass).c_str());
        printf(" %s (svm)!", classIntToString(klass2).c_str());
        printf(" %s (knn)!\n", classIntToString(klass3).c_str());
    }
    
    cvSave("centers.dat", centers);
    bayes.save("bayes.dat");
       
        //this->saveTrainingFile(trainingFile, &values);
    
    // TRAINING!
    
    /*cout << "Training to be a pro! (We need a montage) " << endl << "* * * 80s Music begins playing... * * *" << endl;
    
    double diff = 1;
    for (int e = 0; e < 10000 && diff > 0.0001; e++)
    {
        for (unsigned int i = 0; i < values.size(); i++)
        {
            _regressor->add_to_batch(values[i]); // Train on one set of scores. All features on one training image.
        }
        diff = fabs(_regressor->gradient_decent()); // Process batch (gradient vector)
    }
    
    cout << endl << endl << "All read up. Now, burn the books!" << endl;
    
    for (unsigned int i = 0; i < values.size(); i++)
    {
        delete[] values[i].values;
    }
    
    cout << endl << endl << "I'm ready. Let's DO THIS!" << endl;*/
    
    return true;
}
