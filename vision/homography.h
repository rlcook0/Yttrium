#include "cv.h"
#include "cxcore.h"
#include "ml.h"

#include "utils.h"
#include "objects.h"

#define NUM_TRIALS 10
#define NUM_PAIRS  4

class Homography {
public:
    Homography();
    ~Homography();
    
    CvMat *compute(CvMat *, CvMat *);
    int num_inliers();
    const CvMat *best_h();
    
    threshold = 5.0;
private:
    CvMat *points1, *points2;
    int largestInlierSet;
    CvMat *bestH;
};