#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sys/stat.h>
 
#include "highgui.h"
#include "cv.h"
#include "cxcore.h"

#include "homography.h"

Homography::Homography() {
    largestInlierSet = 0;
    threshold = 0.1;
}

Homography::~Homography() {
    // waaa
}

CvMat *Homography::compute(CvMat *pt1, CvMat *pt2) {
    assert(pt1->rows == pt2->rows);
    
    points1 = cvCloneMat(pt1);
    points2 = cvCloneMat(pt2);
    
    CvMat *assn1 = cvCreateMat(pt1->rows, 3, CV_32F),
          *assn2 = cvCreateMat(pt1->rows, 3, CV_32F),
          *allresult = cvCreateMat(3, pt1->rows, CV_32F),
          *onept = cvCreateMat(3, 1, CV_32F);
    cvSet(assn1, cvScalar(1));
    cvSet(assn2, cvScalar(1));
    
    
    if (pt1->rows < NUM_PAIRS || pt2->rows < NUM_PAIRS) return NULL;
    
    int match1[NUM_PAIRS], match2[NUM_PAIRS];
    CvMat *newpoints1 = cvCreateMat(NUM_PAIRS, 3, CV_32F), 
          *newpoints2 = cvCreateMat(NUM_PAIRS, 3, CV_32F),
          *np1t = cvCreateMat(3, NUM_PAIRS, CV_32F),
          *np2t = cvCreateMat(3, NUM_PAIRS, CV_32F),
          *result = cvCreateMat(3, NUM_PAIRS, CV_32F);
    cvSet(newpoints1, cvScalar(1));
    cvSet(newpoints2, cvScalar(1));
    
    
    CvMat *P = cvCreateMat(3, NUM_PAIRS, CV_32F); 
    CV_MAT_ELEM(*P, float, 0, 2) = 1;
    CvMat *Pp =cvCreateMat(1, 3, CV_32F);
          
    CvMat *H = cvCreateMat(3, 3, CV_32F);
    CvMat *A = cvCreateMat(NUM_PAIRS * 2, 9, CV_32F);
    //CvMat *W = cvCreateMat(NUM_PAIRS * 2, 9, CV_32F);
    CvMat *C = cvCreateMat(9, 1, CV_32F);
    for (int trial = 0; trial < NUM_TRIALS; ++trial) {
        // Pick a random correspondence
        vector<int> randpts1(pt1->rows), randpts2(pt2->rows);
        for (int i = 0; i < pt1->rows; ++i) {
            randpts1[i] = i;
            randpts2[i] = i;
        }
        
        for (int i = 0; i < pt1->rows; ++i) {
            int rand1 = rand() % randpts1.size();
            int rand2 = rand() % randpts2.size();
            
            int idx1 = randpts1[rand1];
            int idx2 = randpts2[rand2];
            
            randpts1.erase(randpts1.begin()+rand1);
            randpts2.erase(randpts2.begin()+rand2);
            
            CV_MAT_ELEM(*assn1, float, i, 0) = CV_MAT_ELEM(*pt1, float, idx1, 0);
            CV_MAT_ELEM(*assn1, float, i, 1) = CV_MAT_ELEM(*pt1, float, idx1, 1);
            CV_MAT_ELEM(*assn2, float, i, 0) = CV_MAT_ELEM(*pt2, float, idx2, 0);
            CV_MAT_ELEM(*assn2, float, i, 1) = CV_MAT_ELEM(*pt2, float, idx2, 1);
        }
        
        for (int i = 0; i < NUM_PAIRS; ++i) {
            CV_MAT_ELEM(*newpoints1, float, i, 0) = CV_MAT_ELEM(*assn1, float, i, 0);
            CV_MAT_ELEM(*newpoints1, float, i, 1) = CV_MAT_ELEM(*assn1, float, i, 1);
            CV_MAT_ELEM(*newpoints2, float, i, 0) = CV_MAT_ELEM(*assn2, float, i, 0);
            CV_MAT_ELEM(*newpoints2, float, i, 1) = CV_MAT_ELEM(*assn2, float, i, 1);
        }
        
        // Compute H
        cvSet(A, cvScalar(0));
        //int origrow = 0;
        for (int row = 0; row < 2 * NUM_PAIRS; ++row) {
            if (row % 2 == 0) {
                int theRow = row / 2;
                CV_MAT_ELEM(*A, float, row, 0) = CV_MAT_ELEM(*newpoints1, float, theRow, 0);
                CV_MAT_ELEM(*A, float, row, 1) = CV_MAT_ELEM(*newpoints1, float, theRow, 1);
                CV_MAT_ELEM(*A, float, row, 2) = 1;
                CV_MAT_ELEM(*A, float, row, 6) = -CV_MAT_ELEM(*newpoints1, float, theRow, 0) * CV_MAT_ELEM(*newpoints2, float, theRow, 0);
                CV_MAT_ELEM(*A, float, row, 7) = -CV_MAT_ELEM(*newpoints1, float, theRow, 1) * CV_MAT_ELEM(*newpoints2, float, theRow, 1);
                CV_MAT_ELEM(*A, float, row, 8) = CV_MAT_ELEM(*newpoints2, float, theRow, 0);
            } else {
                int theRow = (row - 1) / 2;
                CV_MAT_ELEM(*A, float, row, 3) = CV_MAT_ELEM(*newpoints1, float, theRow, 0);
                CV_MAT_ELEM(*A, float, row, 4) = CV_MAT_ELEM(*newpoints1, float, theRow, 1);
                CV_MAT_ELEM(*A, float, row, 5) = 1;
                CV_MAT_ELEM(*A, float, row, 6) = -CV_MAT_ELEM(*newpoints1, float, theRow, 0) * CV_MAT_ELEM(*newpoints2, float, theRow, 0);
                CV_MAT_ELEM(*A, float, row, 7) = -CV_MAT_ELEM(*newpoints1, float, theRow, 1) * CV_MAT_ELEM(*newpoints2, float, theRow, 1);
                CV_MAT_ELEM(*A, float, row, 8) = CV_MAT_ELEM(*newpoints2, float, theRow, 0);
            }
        }
        
         // m = mean(fp[:2], axis=1)
    // maxstd = max(std(fp[:2], axis=1))
    // C1 = diag([1/maxstd, 1/maxstd, 1]) 
    // C1[0][2] = -m[0]/maxstd
    // C1[1][2] = -m[1]/maxstd
    // fp = dot(C1,fp)
        

    // #--to points--
    // m = mean(tp[:2], axis=1)
    // #C2 = C1.copy() #must use same scaling for both point sets
    // maxstd = max(std(tp[:2], axis=1))
    // C2 = diag([1/maxstd, 1/maxstd, 1])
    // C2[0][2] = -m[0]/maxstd
    // C2[1][2] = -m[1]/maxstd
    // tp = dot(C2,tp)


    // #create matrix for linear method, 2 rows for each correspondence pair
    // nbr_correspondences = fp.shape[1]
    // A = zeros((2*nbr_correspondences,9))
    // for i in range(nbr_correspondences):        
        // A[2*i] = [-fp[0][i],-fp[1][i],-1,0,0,0,tp[0][i]*fp[0][i],tp[0][i]*fp[1][i],tp[0][i]]
        // A[2*i+1] = [0,0,0,-fp[0][i],-fp[1][i],-1,tp[1][i]*fp[0][i],tp[1][i]*fp[1][i],tp[1][i]]

    // U,S,V = linalg.svd(A)

    // H = V[8].reshape((3,3))    

    // #decondition
    // H = dot(linalg.inv(C2),dot(H,C1))

    // #normalize and return
    // return H / H[2][2]
        
        
        CvMat *B = cvCreateMat(9, 1, CV_32F);
        cvSet(B, cvScalar(0));
        
        int ok = cvSolve(A, B, C, CV_SVD);
        if (!ok) continue;
        
        for (int i = 0; i < 9; ++i) {
            CV_MAT_ELEM(*H, float, i / 3, i % 3) = CV_MAT_ELEM(*C, float, i, 0);
        }
        
        // Estimate w
        double w;
        cvTranspose(newpoints1, np1t);
        cvTranspose(newpoints2, np2t);
        
        cvGEMM(H, np1t, 1, np2t, -1, result, 0);
        w = cvAvg(result).val[0];
        printf("w: %f\n", w);
        
        cvGEMM(H, assn1, w, assn2, -1, allresult, CV_GEMM_B_T+CV_GEMM_C_T);
        
        // Check inliers, outliers
        vector<bool> classes(pt1->rows);
        int num_inliers = 0;
        for (int i = 0; i < pt1->rows; ++i) {
            double err = CV_MAT_ELEM(*allresult, float, 0, i) 
                       + CV_MAT_ELEM(*allresult, float, 1, i) 
                       + CV_MAT_ELEM(*allresult, float, 2, i);
            classes[i] = err < threshold;
            num_inliers += err < threshold ? 1 : 0;
        }
        
        if (num_inliers > largestInlierSet) {
            largestInlierSet = num_inliers;
            printf("new best %d/%d on trial %d\n", num_inliers, pt1->rows, trial);
            
            cvGEMM(H, assn1, w, NULL, 0, allresult, CV_GEMM_B_T+CV_GEMM_C_T);
            printf("%f %f %f %f %f\n", w, CV_MAT_ELEM(*assn2, float, 5, 0), CV_MAT_ELEM(*assn2, float, 5, 1),
                CV_MAT_ELEM(*allresult, float, 0, 5), CV_MAT_ELEM(*allresult, float, 1, 5));
            
            bestH = cvCloneMat(H);
        }
    }
    
    return bestH;
}

int Homography::num_inliers() {
    return largestInlierSet;
}

const CvMat *Homography::best_h() {
    return bestH;
}