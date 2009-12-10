#include "cv.h"
#include "cxcore.h"
#include "ml.h"

#include "utils.h"
#include "objects.h"

#define NUM_TRIALS 1000
#define NUM_PAIRS  4

class Homography {
public:
    Homography();
    ~Homography();
    
    CvMat *compute(CvMat *, CvMat *);
    int num_inliers();
    const CvMat *best_h();
    
    double threshold;
private:
    CvMat *points1, *points2;
    int largestInlierSet;
    CvMat *bestH;
};