#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sys/stat.h>
 
#include "highgui.h"
#include "cv.h"
#include "cxcore.h"

Homography::Homography() {
    largestInlierSet = 0;
    threshold = 5.0;
}

Homography::~Homography() {
    // waaa
}

CvMat *Homography::compute(CvMat *pt1, CvMat *pt2) {
    assert(pt1->rows == pt2->rows);
    
    points1 = cvCloneMat(pt1);
    points2 = cvCloneMat(pt2);
    
    CvMat assn1 = cvMat(pt1->rows, 3, CV_32F),
          assn2 = cvMat(pt1->rows, 3, CV_32F)
          allresult = cvMat(3, pt1->rows, CV_32F),
          onept = cvMat(3, 1, CV_32F);
    cvSet(&assn1, cvScalar(1));
    cvSet(&assn2, cvScalar(1));
    
    
    if (pt1->rows < NUM_PAIRS || pt2->rows < NUM_PAIRS) return NULL;
    
    int match1[NUM_PAIRS], match2[NUM_PAIRS];
    CvMat newpoints1 = cvMat(NUM_PAIRS, 3, CV_32F), 
          newpoints2 = cvMat(NUM_PAIRS, 3, CV_32F),
          np1t = cvMat(3, NUM_PAIRS, CV_32F),
          np2t = cvMat(3, NUM_PAIRS, CV_32F),
          result = cvMat(3, NUM_PAIRS, CV_32F);
    cvSet(&newpoints1, cvScalar(1));
    cvSet(&newpoints2, cvScalar(1));
    
    
    CvMat P = cvMat(3, NUM_PAIRS, CV_32F); 
    CV_MAT_ELEM(P, double, 0, 2) = 1;
    CvMat Pp = cvMat(1, 3, CV_32F);
          
    CvMat H = cvMat(3, 3, CV_32F);
    CvMat A = cvMat(NUM_PAIRS * 2, 9, CV_32F);
    CvMat W = cvMat(NUM_PAIRS * 2, 9, CV_32F);
    CvMat C = cvMat(NUM_PAIRS * 2, 1, CV_32F);
    double w;
    for (int trial = 0; trial < NUM_TRIALS; ++trial) {
        // Pick a random correspondence
        vector<int> randpts1(pt1->rows), randpts2(pt2->rows);
        for (int i = 0; i < pt1->rows; ++i) {
            randpts1[i] = i;
            randpts2[i] = i;
        }
        
        for (int i = 0; i < pt1->rows; ++i) {
            int rand1 = rand() % randpts1.size();
            int rand2 = rand() & randpts2.size();
            
            int idx1 = randpts1[rand1];
            int idx2 = randpts2[rand2];
            
            randpts1.erase(randpts1.begin()+rand1);
            randpts2.erase(randpts2.begin()+rand2);
            
            CV_MAT_ELEM(assn1, double, i, 0) = CV_MAT_ELEM(pt1, double, idx1, 0);
            CV_MAT_ELEM(assn1, double, i, 1) = CV_MAT_ELEM(pt1, double, idx1, 1);
            CV_MAT_ELEM(assn2, double, i, 0) = CV_MAT_ELEM(pt2, double, idx2, 0);
            CV_MAT_ELEM(assn2, double, i, 1) = CV_MAT_ELEM(pt2, double, idx2, 1);
        }
        
        for (int i = 0; i < NUM_PAIRS; ++i) {
            CV_MAT_ELEM(newpoints1, double, i, 0) = CV_MAT_ELEM(assn1, double, i, 0);
            CV_MAT_ELEM(newpoints1, double, i, 1) = CV_MAT_ELEM(assn1, double, i, 1);
            CV_MAT_ELEM(newpoints2, double, i, 0) = CV_MAT_ELEM(assn2, double, i, 0);
            CV_MAT_ELEM(newpoints2, double, i, 1) = CV_MAT_ELEM(assn2, double, i, 1);
        }
        
        // Compute H
        cvSet(&A, cvScalar(0));
        int origrow = 0;
        for (int row = 0; row < 2 * NUM_PAIRS; ++row) {
            if (row % 2 == 1) {
                CV_MAT_ELEM(A, double, row, 0) = CV_MAT_ELEM(newpoints1, double, row, 0);
                CV_MAT_ELEM(A, double, row, 1) = CV_MAT_ELEM(newpoints1, double, row, 1);
                CV_MAT_ELEM(A, double, row, 2) = 1;
                CV_MAT_ELEM(A, double, row, 6) = -CV_MAT_ELEM(newpoints1, double, row, 0) * CV_MAT_ELEM(newpoints2, double, row, 0);
                CV_MAT_ELEM(A, double, row, 7) = -CV_MAT_ELEM(newpoints1, double, row, 1) * CV_MAT_ELEM(newpoints2, double, row, 1);
                CV_MAT_ELEM(A, double, row, 8) = CV_MAT_ELEM(newpoints2, double, row, 0);
            } else {
                CV_MAT_ELEM(A, double, row, 3) = CV_MAT_ELEM(newpoints1, double, row - 1, 0);
                CV_MAT_ELEM(A, double, row, 4) = CV_MAT_ELEM(newpoints1, double, row - 1, 1);
                CV_MAT_ELEM(A, double, row, 5) = 1;
                CV_MAT_ELEM(A, double, row, 6) = -CV_MAT_ELEM(newpoints1, double, row - 1, 0) * CV_MAT_ELEM(newpoints2, double, row - 1, 0);
                CV_MAT_ELEM(A, double, row, 7) = -CV_MAT_ELEM(newpoints1, double, row - 1, 1) * CV_MAT_ELEM(newpoints2, double, row - 1, 1);
                CV_MAT_ELEM(A, double, row, 8) = CV_MAT_ELEM(newpoints2, double, row - 1, 0);
            }
        }
        cvSVD(&A, &W);
        cvGetCol(&W, &C, W->cols-1);
        for (int i = 0; i < 9; ++i) {
            CV_MAT_ELEM(H, double, i / 3, i % 3) = CV_MAT_ELEM(C, double, i, 0);
        }
        
        // Estimate w
        double w;
        cvTranspose(&newpoints1, &np1t);
        cvTranspose(&newpoints2, &np2t);
        
        cvGEMM(&H, &np1t, 1, &np2t, -1, &result, 0);
        w = cvAvg(&result).val[0];
        
        cvGemm(&H, &assn1, w, &assn2, -1, &allresult, CV_GEMM_B_T+CV_GEMM_C_T);
        
        // Check inliers, outliers
        vector<bool> classes(pt1->rows);
        int num_inliers = 0;
        for (int i = 0; i < pt1->rows; ++i) {
            double err = CV_MAT_ELEM(allresult, double, 0, i) 
                       + CV_MAT_ELEM(allresult, double, 1, i) 
                       + CV_MAT_ELEM(allresult, double, 2, i);
            classes[i] = err < thresh;
            num_inliers += err < thresh ? 1 : 0;
        }
        
        if (num_inliers > largestInlierSet) {
            largestInlierSet = num_inliers;
            bestH = cvCloneMat(&H);
        }
    }
}

int Homography::num_inliers() {
    return largestInlierSet;
}

const CvMat *Homography::best_h() {
    return bestH;
}