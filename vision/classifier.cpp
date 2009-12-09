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

#include <sys/stat.h>
 
#include "highgui.h"

#include "surf.h"
#include "surflib.h"

#include "cv.h"
#include "cxcore.h"
 
#include "classifier.h"
#include "logreg.h"

#define BAYES_ON false
#define SVM_ON false
#define KNN_ON false
#define RTREE_ON true
#define MUGTREE_ON false

#define LOAD_KMEANS false
#define SAVE_KMEANS true
#define LOAD_IPOINTS true
#define SAVE_IPOINTS true

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
    

    if (BAYES_ON) bayes.load("bayes.dat");
    if (SVM_ON)   svm.load("svm.dat");
    if (KNN_ON)   knn.load("knn.dat");
    centers = (CvMat *)cvLoad("centers.dat");
        
    //loadSURFFile(filename);
    //setupKDTree();
    
    return true;
}
 
// saveState
// Writes classifier configuration to the given file
bool Classifier::saveState(const char *filename)
{
    assert(filename != NULL);
    
    //saveSURFFile(filename);
    
    return true;
}
 
 
// saveState
// Writes classifier TrainingFile to the given file
bool Classifier::saveSURFFile(const char *filename)
{
    if (filename == NULL) return false;
    
    ofstream outfile;
    outfile.open(filename);
    
    outfile << surfFeatures.size() << endl;
    
    for(map<string, vector<Ipoint> >::iterator it = surfFeatures.begin(); it != surfFeatures.end(); ++it)
    {
        outfile << it->second.size() << " " << it->first << endl; 
        for (unsigned j = 0; j < it->second.size(); j++)
        {
            Ipoint i = (it->second)[j];
            outfile << i.x << " " << i.y << " " << i.scale << " " << i.orientation << " " << i.laplacian << " ";
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


bool Classifier::loadSURFFile(const char *filename)
{
    if (filename == NULL) return false;
    
    ifstream infile;
    infile.open(filename);
    
    int count, ipoints;
    
    infile >> count; infile.ignore(1); //\n
    
    for (int c = 0; c < count; c++) 
    {
	    
        string name;
        infile >> ipoints; infile.ignore(1);        
        infile >> name;    infile.ignore(1);
        
        printf("reading %s: %d\n", name.c_str(), ipoints);
        
        vector<Ipoint> v;
        for (int j = 0; j < ipoints; j++) 
        {
            Ipoint i;
            infile >> i.x >> i.y >> i.scale >> i.orientation >> i.laplacian;
            for (int k = 0; k < 64; k++)
                infile >> i.descriptor[k];
            
            v.push_back(i);   
            infile.ignore(1); //\n
        }
        surfFeatures[name] = v;
        infile.ignore(1);  // \n
    }
    infile.close();
    
    cout << "Loaded SURF descriptor values from: " << filename << endl;
    
    
    int total = 0;
    for (map<string, vector<Ipoint> >::const_iterator it = surfFeatures.begin(); it != surfFeatures.end(); ++it) {
        total += it->second.size();
        printf("%s %d\n", it->first.c_str(), total);
        surfThresh.push_back(pair<string, int>(it->first, total));
        surfTotal[it->first] = (int)(it->second.size());
    }
    surfTotalIpoints = total;
    printf("how many? %d\n", total);
    
    return true;
}


bool Classifier::setupKDTree()
{
    CvMat *desc = cvCreateMat(surfTotalIpoints, 64, CV_32FC1);

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
    surfFT = cvCreateKDTree(desc);
    printf("made surf tree!\n");
    
    return true;
}


string Classifier::indexToClass(int index) 
{
    for (unsigned i = 0; i < surfThresh.size(); ++i)
        if (index < surfThresh[i].second)
            return surfThresh[i].first;
    
    return "ERROR";
}

int Classifier::indexToClassInt(int index) 
{
    for (unsigned i = 0; i < surfThresh.size(); ++i)
        if (index < surfThresh[i].second)
            return (int)i;
    
    return -1;
}

int Classifier::stringToClassInt(string type) 
{
    if (type == "other")    return kOther;
    if (type == "mug")      return kMug;
    if (type == "keyboard") return kKeyboard;
    if (type == "clock")    return kClock;
    if (type == "stapler")  return kStapler;
    if (type == "scissors") return kScissors;
    
    return -1;
}

string Classifier::classIntToString(int type) 
{
    if (type == kOther)     return "other";
    if (type == kMug)       return "mug";
    if (type == kKeyboard)  return "keyboard";
    if (type == kClock)     return "clock";
    if (type == kStapler)   return "stapler";
    if (type == kScissors)  return "scissors";
    
    return "error";
}

CvMat *normalize(const CvMat* vector) {
    CvMat *norm = cvCloneMat(vector);
    double normVal = cvNorm(vector);
    cvScale(vector, norm, 1 / normVal);
    
    return norm;
}

// TESTING ONLY
bool Classifier::showRect(IplImage *image, CObject *rect, const vector<Ipoint> *pts = NULL) {
    //char *WINDOW_NAME = "test";
    CvFont font;

    IplImage *frameCopy = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
    cvConvertImage(image, frameCopy);
        
    cvNamedWindow("test", CV_WINDOW_AUTOSIZE);
    cvInitFont(&font, CV_FONT_VECTOR0, 0.75, 0.75, 0.0f, 1, CV_AA);
    
    if (pts != NULL) {
        CvScalar color = CV_RGB(255, 255, 255);
        for (int i = 0; i < (int)pts->size(); ++i) {
            
            cvRectangle(image, cvPoint((*pts)[i].x, (*pts)[i].y), 
                cvPoint((*pts)[i].x + 3, (*pts)[i].y + 3), color);
        }
    }
    
    rect->draw(frameCopy, CV_RGB(255, 0, 255), &font);
    
    
    cvShowImage("test", frameCopy);
    cvReleaseImage(&frameCopy);

    return cvWaitKey(10) != -1;
}
// run
// Runs the classifier over the given frame and returns a list of
// objects found (and their location).
// you only need to populate "objects" if "scored" is true. if you do
// populate "objects" when "scored" is false, it will not affect your 
// score but it may be helpful to you for debugging (to visualize your
// results on more frames)
bool Classifier::run(const IplImage *frame, CObjectList *objects, bool scored)
{
    if (!scored)
        return true;
    
    cout << "--------------------------------------" << endl;
    cout << "\t\tRun" << endl;
    
    assert((frame != NULL) && (objects != NULL));
    
    printf("Let's go!\n");
    
    // Convert to grayscale.
    IplImage *gray  = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
    cvCvtColor(frame, gray, CV_BGR2GRAY);
    
    // Resize by half first, as per the handout.
    double scale = 1.0;
    IplImage *dst = cvCreateImage(cvSize(gray->width  / scale, gray->height  / scale), gray->depth,  gray->nChannels);
    cvResize(gray, dst);

    printf("About to do SURF\n");
    
    std::vector<Ipoint> pts;
    surfDetDes(dst, pts, false);
    
    printf("Done SURF\n");
    
    /*CvMat* descriptors = cvCreateMat(pts.size(), 64, CV_32FC1);
    for (int i = 0; i < (int)pts.size(); ++i) {
        Ipoint* ipt = &(pts[i]);
        for (int j = 0; j < 64; ++j) {
            //printf(" (%d %d %f)", i, j, ipt->descriptor[j]);
            cvSetReal2D(descriptors, i, j, ipt->descriptor[j]);
        }
    }
*/
    //int k = 5;
    //CvMat *indicies = cvCreateMat(pts.size(), k, CV_32SC1), *distances = cvCreateMat(pts.size(), k, CV_64FC1);
    //cvFindFeatures(surfFT, descriptors, indicies, distances, k);

    printf("Clustering...\n");
    
    vector<int> cluster(pts.size());
    for (int i = 0; i < (int)pts.size(); ++i) {
        double min_distance = DBL_MAX;
        int min_index = -1;
        for (int c = 0; c < NUM_CLUSTERS; ++c) {
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
    
    /*printf("cluster\n");
    for (int i = 0; i < (int)pts.size(); i++) {
        printf("%d ", cluster[i]);
    }
    printf("\n");*/
    
    printf("Done clustering...\n");
    
    run_boxscan(dst, cluster, pts);

    cvReleaseImage(&gray);
    return true;
}


bool Classifier::run_boxscan(IplImage *dst, vector<int> &cluster, vector<Ipoint> &pts)
{
    float scale = 4.0f;
    int maxWidth = dst->width;
    int maxHeight = dst->height;
    
    int numLayers = 7;
    while(numLayers-- > 0) {
        for (int x = 0; x < maxWidth - 32*scale; x += 8 * scale) {
            for (int y = 0; y < maxHeight - 32*scale; y += 8 * scale) {
                std::vector<int> newpts;
                for (int i = 0; i < (int)pts.size(); ++i) {
                    if (pts[i].x >= x && pts[i].x < x + 32*scale && pts[i].y >= y && pts[i].y < y + 32*scale)
                        newpts.push_back(i);
                }
        
                if (newpts.size() < MIN_IPOINTS) continue;
        
                CvMat *query = cvCreateMat(1, NUM_CLUSTERS, CV_32FC1);
                cvSet(query, cvScalar(0));
                for (int row = 0; row < (int)newpts.size(); ++row) {
                    int idx = newpts[row];
                    int cluster_idx = cluster[idx];

                    int oldVal = cvGetReal2D(query, 0, cluster_idx);
                    cvSetReal2D(query, 0, cluster_idx, oldVal+1);
                    
                    
                }
                
                // TEST?
                CvMat* normalized = normalize(query);
                query = normalized;
                
                // printf("\n");
                // for (int i = 0; i < NUM_CLUSTERS; i++) {
                    // printf("%f ", cvGetReal2D(query, 0, i));
                // }
                // printf("\n");
        
                int klass, klass2, klass3;
                
                if (BAYES_ON) klass = bayes.predict(query);
                if (SVM_ON)   klass2 = svm.predict(query);
                if (KNN_ON)   klass3 = knn.find_nearest(query, 5);
                
                cout << "I think it's a ";
                if (BAYES_ON) cout << classIntToString(klass) << "(bayes) ";
                if (SVM_ON)   cout << classIntToString(klass2) << "(svm) ";
                if (KNN_ON)   cout << classIntToString(klass3) << "(knn) ";
                cout << endl;
                        
                cvResetImageROI(dst);
        
                CObject o;
                o.rect = cvRect(x, y, 32 * scale, 32 * scale);;
                o.label = classIntToString(klass2);
        
                showRect(dst, &o, &pts);
            }
        }
        
        scale *= 1.2;
    }
    return true;
}

bool FileExists(string strFilename) {
  struct stat stFileInfo;
  bool blnReturn;
  int intStat;

  // Attempt to get the file attributes
  intStat = stat(strFilename.c_str(),&stFileInfo);
  if(intStat == 0) {
    blnReturn = true;
  } else {
    blnReturn = false;
  }
  
  return(blnReturn);
}
 
// train
// Trains the classifier to recognize the objects given in the
// training file list.
bool Classifier::train(TTrainingFileList& fileList, const char *trainingFile)
{
    cout << "------------------------------------------" << endl;
    cout << "\t\tTraining" << endl;

    if (!LOAD_IPOINTS || !FileExists("/tmp/surf/num_images")) {
        extract(fileList, trainingFile);
    } else {
        int numImages;
        
        ifstream num;
        num.open("/tmp/surf/num_images");
        num >> numImages;
        num.close();
        
        printf("Loading %d saved images...\n", numImages);
        
        surfTotalIpoints = 0;
        for (int i = 0; i < numImages; ++i) {
            showProgress(i, numImages);
            char klassfn[50], datfn[50];
            sprintf(klassfn, "/tmp/surf/img%d.klass", i);
            sprintf(datfn, "/tmp/surf/img%d.dat", i);
            
            string klass;
            
            ifstream klassf;
            klassf.open(klassfn);
            klassf >> klass;
            klassf.close();
            
            vector<Ipoint> pts;
            loadSurf(datfn, pts);
            
            surfTotalIpoints += (int)pts.size();
            
            allImages.push_back(pair<string, vector<Ipoint> >(klass, pts));
        }
        
        printf("Done!\n");
    }
    
    if (!LOAD_IPOINTS && SAVE_IPOINTS) {
        printf("Saving %d images...\n", (int)allImages.size());
        for (int i = 0; i < (int)allImages.size(); ++i) {
            showProgress(i, (int)allImages.size());
            
            char klassfn[50], datfn[50];
            sprintf(klassfn, "/tmp/surf/img%d.klass", i);
            sprintf(datfn, "/tmp/surf/img%d.dat", i);
            
            saveSurf(datfn, allImages[i].second);
            
            ofstream klass;
            klass.open(klassfn);
            klass << allImages[i].first;
            klass.close();
        }
        
        ofstream num;
        num.open("/tmp/surf/num_images");
        num << (int)allImages.size();
        num.close();
        printf("Done!\n");
    }
    
    // make folds
    vector<int> indexes;
    for (int i = 0; i < (int)allImages.size(); ++i) indexes.push_back(i);
    random_shuffle ( indexes.begin(), indexes.end() );
    
    int size = (int)((double)allImages.size() * 0.7);
    trainSet = new vector<pair<string, vector<Ipoint> > >(size);
    testSet = new vector<pair<string, vector<Ipoint> > >((int)allImages.size() - size);
    
    for (int i = 0; i < (int)allImages.size(); ++i) {
        if (i < size)
            trainSet->push_back(allImages[i]);
        else
            testSet->push_back(allImages[i]);
    }
    
    // testSet = trainSet = &allImages;
    
    CvMat *desc, *responses;
    if(!LOAD_KMEANS || !FileExists("desc.dat")) {
        desc = cvCreateMat(trainSet->size(), NUM_CLUSTERS, CV_32FC1);
        responses = cvCreateMat(trainSet->size(), 1, CV_32SC1);

        cout << "massaging responses..." << endl;
        for (int i = 0; i < (int)allImages.size(); ++i) 
        {
            int klass = stringToClassInt(allImages[i].first);
            cvSetReal1D(responses, i, klass);
        }

        train_kmeans(desc);
    } else {
        printf("Loading kmeans...");
        desc = (CvMat *)cvLoad("desc.dat");
        centers = (CvMat *)cvLoad("centers.dat");
        responses = (CvMat *)cvLoad("responses.dat");
        printf(" Loaded!\n");
    }
    
    if (!LOAD_KMEANS && SAVE_KMEANS) {
        printf("Saving desc, kmeans, and responses...");
        cvSave("desc.dat", desc);
        cvSave("responses.dat", responses);
        cvSave("centers.dat", centers);
        printf(" Saved!\n");
    }

    train_bayes(desc, responses);
    train_svm(desc, responses);
    train_knn(desc, responses);
    train_rtree(desc, responses);
    train_mugtree(desc, responses);
    
    /*printf("Saving classifiers...");
    if (BAYES_ON) bayes.save("bayes.dat");
    if (SVM_ON) svm.save("svm.dat");
    if (KNN_ON) knn.save("knn.dat");
    if (RTREE_ON) rtree.save("rtree.dat");
    printf(" Saved!\n");
    */
    train_test();
    
    return true;
}


bool Classifier::train_kmeans(CvMat *desc) 
{
    cout << "------------------------------------------" << endl;
    cout << "Step Start: kmeans" << endl;
    
    cout << "setting first descriptors..." << endl;
    
    int trainIpoints = 0;
    for (int i = 0; i < (int)trainSet->size(); ++i) {
        trainIpoints += (*trainSet)[i].second.size();
    }
    
    CvMat *all_desc = cvCreateMat(trainIpoints, 64, CV_32FC1);
    int where = 0;
    for (int i = 0; i < (int)allImages.size(); ++i) {
        for (int iptNo = 0; iptNo < (int)(*trainSet)[i].second.size(); ++iptNo) {
            //printf("%d %d\n", i, iptNo);
            Ipoint *pt = &((*trainSet)[i].second[iptNo]);
            for (int j = 0; j < 64; ++j) {
                cvSetReal2D(all_desc, where, j, pt->descriptor[j]);
            }
            where++;
        }
    }
    
    cout << "kmeans starting..." << endl;
    
    CvMat *clusters = cvCreateMat(trainIpoints, 1, CV_32SC1);
    centers = cvCreateMat(NUM_CLUSTERS, 64, CV_32FC1);
    cvKMeans2(all_desc, // samples
        NUM_CLUSTERS, // clusters
        clusters, // labels
        cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 0.1), // termcrit
        1, // attempts
        &rng, //rng
        0, // flags
        centers, // centers
        NULL); // compactness*/
    //int cvKMeans2(const CvArr* samples, int nclusters, CvArr* labels, CvTermCriteria termcrit,
//    int attempts=1, CvRNG* rng=0, int flags=0, CvArr* centers=0, double* compactness=0)¶
    
    cout << "creating new descriptors..." << endl;
    
    cvSet(desc, cvRealScalar(0));
    where = 0;
    for(int i = 0; i < (int)(*trainSet).size(); ++i) {
        for (int iptNo = 0; iptNo < (int)(*trainSet)[i].second.size(); ++iptNo) {
            int cluster = cvGetReal1D(clusters, where);
            
            //printf("(%d %d)\n", i, cluster);
            int oldVal = cvGetReal2D(desc, i, cluster);
            cvSetReal2D(desc, i, cluster, oldVal+1);
            ++where;
        }
        
        // normalize
        double total = 0;
        for (int j = 0; j < NUM_CLUSTERS; ++j) {
            double val = cvGetReal2D(desc, i, j);
            total += val*val;
        }
        double norm = sqrt(total);
        for (int j = 0; j < NUM_CLUSTERS; ++j) {
            double val = cvGetReal2D(desc, i, j);
            cvSetReal2D(desc, i, j, val / norm);
        }
    }
    
    cout << "Step Done:  kmeans" << endl; 
    return true;
}

bool Classifier::train_bayes(CvMat *desc, CvMat *responses) 
{
 
    cout << "------------------------------------------" << endl;
    cout << "Step Start: bayes" << endl; 
    
    
    
    if (BAYES_ON) bayes.train(desc, responses);
    else cout << "Bayes is turned off" << endl;
    
    return true;
}

bool Classifier::train_svm(CvMat *desc, CvMat *responses) 
{
    cout << "------------------------------------------" << endl;
    cout << "Step Start: svm" << endl; 
    
    if (SVM_ON) svm.train(desc, responses);
    else cout << "SVM is turned off" << endl;
    
    cout << "Step Done:  svm" << endl;
    return true;
}

bool Classifier::train_knn(CvMat *desc, CvMat *responses) 
{
    cout << "------------------------------------------" << endl;
    cout << "Step Start: knn" << endl; 
    
    if (KNN_ON) knn.train(desc, responses);
    else cout << "KNN is turned off" << endl;
    
    cout << "Step Done:  knn" << endl;
    return true;
}


bool Classifier::train_rtree(CvMat *desc, CvMat *responses) 
{
    cout << "------------------------------------------" << endl;
    cout << "Step Start: rtree" << endl; 
    
    CvRTParams params( 
        20,                                 //Max depth 
        5,                                  //Min sample count 
        0,                                  //Regression accuracy 
        false,                              //Use Surrogates 
        15,                                 //Max Categories 
        NULL,                               //Priors  
        false,                              //Calculate Variables variance 
        0,                                  //Nactive Vars 
        50,                                 //Max Tree Count 
        0.1,                                //Accuracy to archive 
        CV_TERMCRIT_EPS+CV_TERMCRIT_ITER    //Criteria to stop algorithm 
    );
    
    printf("%d %d\n", NUM_CLUSTERS, desc->cols);
    assert(NUM_CLUSTERS == desc->cols);
    
    CvMat *var_type = cvCreateMat(desc->cols + 1, 1, CV_8U);
    cvSet(var_type, cvScalarAll(CV_VAR_NUMERICAL));
    cvSet2D(var_type, desc->cols, 0, cvScalar(CV_VAR_CATEGORICAL));
    
    if (RTREE_ON) rtree.train(
        desc, 
        CV_ROW_SAMPLE,
        responses,
        0,
        0,
        var_type,
        0,
        params
    );
    else cout << "RTREE is turned off" << endl;
    
    cout << "Step Done:  rtree" << endl;
    return true;
}

bool Classifier::train_mugtree(CvMat *desc, CvMat *responses) 
{
    cout << "------------------------------------------" << endl;
    cout << "Step Start: mugtree" << endl; 
    
    CvMat *new_responses = cvCreateMat(responses->rows, 1, CV_32SC1);
    for (int i = 0; i < responses->rows; ++i) {
        int klass = cvGetReal2D(responses, i, 0);
        int newKlass = klass == kMug ? kMug : kOther;
        
        cvSetReal2D(new_responses, i, 0, newKlass);
    }
    
    CvMat* var_type = cvCreateMat( desc->cols + 1, 1, CV_8U );
    cvSet( var_type, cvScalarAll(CV_VAR_ORDERED) );
    cvSetReal1D( var_type, desc->cols, CV_VAR_CATEGORICAL );    
    
    if (MUGTREE_ON) mugTree.train(
        desc, 
        CV_ROW_SAMPLE, 
        new_responses,
        0,
        0,
        0,
        0,
        CvBoostParams( CvBoost::REAL, 100, 0.95, 5, false, 0 )
    );
    else cout << "MUGTREE is turned off" << endl;
    
    cout << "Step Done:  mugtree" << endl;
    return true;
}


void logResult(int *data, int klass, int trueClass) {
    if (klass != kOther) // positive
        if (klass == trueClass) // true
            data[0] += 1; //tp
        else
            data[2] += 1; //fn
    else // negative
        if (klass == trueClass) //true
            data[1] += 1; //tn
        else
            data[3] += 1; //fp
}

void logResult(int *data, int klass, int trueClass, int targetClass) {
    if (trueClass == targetClass)
        if (klass == trueClass)
            data[0] += 1;
        else
            data[2] += 1;
    else
        if (klass == trueClass)
            data[1] += 1;
        else
            data[3] += 1;
}

void printResult(int *data, string name) {
    double accuracy = (double)(data[0] + data[1]) / (double)(data[0] + data[1] + data[2] + data[3]); 
    double precision = (double)data[0] / (double)(data[0] + data[2]);
    double recall = (double)data[0] / (double)(data[0] + data[3]);
    double fscore = 2 * (precision * recall) / (precision + recall);
    
    printf("%s\ttp: %d\tfp: %d\ttn: %d\tfn: %d\n", name.c_str(), data[0], data[1], data[2], data[3]);
    printf("%s\taccuracy: %f\tprecision: %f\trecall: %f\tfscore: %f\n", name.c_str(), accuracy, precision, recall, fscore);
}

bool Classifier::train_test()
{
    cout << "------------------------------------------" << endl;
    cout << "Step Start: test" << endl;
    
    int bayes_results[4], svm_results[4], knn_results[4], rtree_results[4], mugtree_results[4];
    for (int i = 0; i < 4; ++i)
        bayes_results[i] = svm_results[i] = knn_results[i] = rtree_results[i] = mugtree_results[i] = 0;
    

    cout << "Running " << (int)(*testSet).size() << " images." << endl;
    for (int img = 0; img < (int)(*testSet).size(); ++img) {
        showProgress(img, (int)(*testSet).size());
        
        vector<Ipoint> pts = (*testSet)[img].second;
        vector<int> cluster(pts.size());
        for (int i = 0; i < (int)pts.size(); ++i) {
            float min_distance = INT_MAX;
            int min_index = -1;
            for (int c = 0; c < NUM_CLUSTERS; ++c) {
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
        
        CvMat *query = cvCreateMat(1, NUM_CLUSTERS, CV_32FC1);
        cvSet(query, cvScalar(0));
        for (int row = 0; row < (int)pts.size(); ++row) {
            int cluster_idx = cluster[row];

            int oldVal = cvGetReal2D(query, 0, cluster_idx);
            cvSetReal2D(query, 0, cluster_idx, oldVal+1);
        }
        
        // printf("\n");
        // for (int i = 0; i < NUM_CLUSTERS; i++) {
            // printf("%d ", (int)cvGetReal2D(query, 0, i));
        // }
        // printf("\n");
        
        CvMat *normalized = normalize(query);
        query = normalized;
        
        int trueClass = stringToClassInt((*testSet)[img].first);
        
        int klass, klass2, klass3, klass4, klass5;
        if (BAYES_ON) klass = bayes.predict(query);
        if (SVM_ON) klass2 = svm.predict(query);
        if (KNN_ON) klass3 = knn.find_nearest(query, 5);
        if (RTREE_ON) klass4 = rtree.predict(query);
        if (MUGTREE_ON) klass5 = mugTree.predict(query);
        
        // tp, fp, tn, fn
        
        if (BAYES_ON) logResult(bayes_results, klass, trueClass);
        if (SVM_ON) logResult(svm_results, klass2, trueClass);
        if (KNN_ON) logResult(knn_results, klass3, trueClass);
        if (RTREE_ON) logResult(rtree_results, klass4, trueClass);
        if (MUGTREE_ON) logResult(mugtree_results, klass5, trueClass, kMug);
        
        
        
        // cout << (*testSet)[img].first << " is a ";
        // if (BAYES_ON) printf(" %s (bayes)!", classIntToString(klass).c_str());
        // if (SVM_ON)   printf(" %s (svm)!", classIntToString(klass2).c_str());
        // if (KNN_ON)   printf(" %s (knn)!", classIntToString(klass3).c_str());
        // cout << endl;
    }
    
    cout << endl;
    
    if (BAYES_ON) printResult(bayes_results, "bayes");
    if (SVM_ON) printResult(svm_results, "svm");
    if (RTREE_ON) printResult(rtree_results, "rtree");
    if (KNN_ON) printResult(knn_results, "knn");
    if (MUGTREE_ON) printResult(mugtree_results, "mugtree");
    
    cout << "Step Done:  test" << endl;
    return true;
}

bool Classifier::extract(TTrainingFileList& fileList, const char *featuresFile)
{
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

    IplImage *image;
    vector<Ipoint> all;

    int maxOther = 10000;
    int maxImages = INT_MAX;

    cout << "Processing images..." << endl;

    int minfiles = min(maxImages, (int)fileList.files.size());
    for (int i = 0; i < minfiles; i++) 
    {
        // skip too many others...
        if (fileList.files[i].label == "other" && --maxOther < 0) continue;

        // show progress
        if (i % 10 == 0) showProgress(i, minfiles);

        image = cvLoadImage(fileList.files[i].filename.c_str(), 0);
        if (image == NULL) {
            cerr << "ERROR: could not load image " << fileList.files[i].filename.c_str() << endl;
            continue;
        }
        
        // SURF.
        std::vector<Ipoint> pts;
        surfDetDes(image, pts, false);

        for (unsigned j = 0; j < pts.size(); ++j)
            surfFeatures[fileList.files[i].label].push_back(pts[j]);

        allImages.push_back(pair<string, vector<Ipoint> >(fileList.files[i].label, pts));

        cvReleaseImage(&image);
    }

    cout << endl << "Extracted surf features: " << endl;

    int total = 0;
    for (map<string, vector<Ipoint> >::const_iterator it = surfFeatures.begin(); it != surfFeatures.end(); ++it) {
        total += it->second.size();
        cout << it->first.c_str() << " " << (int)it->second.size() << endl;
        surfThresh.push_back(pair<string, int>(it->first, total));
    }
    surfTotalIpoints = total;
    return true;
}


