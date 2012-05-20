
#ifndef __SEGMENTER_H__
#define __SEGMENTER_H__

#include <vector>
#include <math.h>
#include <algorithm>
#include "svm-predict.h"
#include "enumList.h"

using namespace std;

struct Feature 
{	
	double sin;
	double cos;
	double vel; // velocity
};

const int FEATURE_LENGTH = 50;
const int FEATURE_DIM = 2;
const int GESTURE_NUM = 7;
const int SPOTTER_STEP_SIZE = 2;
const int MIN_SEARCH_TH = 10;
const int MAX_SEARCH_TH = 25;
const double GESTURE_SCORE_TH = 0.9;

class Segmenter
{
public:	
	
	vector<GesName> segment(int featureNum, const vector<Feature> &featureBuf);

private:		

	void intplToEqualLength(const vector<Feature> &inVec, vector<Feature> &eqlVec);
	void convertToArray(const vector<Feature> &eqlVec, double* formatedFea);
	GesName outputGesture(const double* gestureScore);	
	
};

#endif