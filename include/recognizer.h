
#ifndef __RECOGNIZER_H__
#define __RECOGNIZER_H__

#include <vector>
#include <iostream>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "svm-predict.h"
#include "segmenter.h"
#include "spotter.h"
#include "utility.h"
#include "tracker.h"

using namespace std;

const double PI = 3.14159265;

// The utility class extend vector<CvPoint> with a counter
// (將 vector<CvPoint> 增加一個計數器的輔助類)
class ptVectorWithCounter
{
public:

	ptVectorWithCounter();

	vector<CvPoint> data;
	int count;	

	void addPoint(CvPoint point);

};

class Recognizer
{
public:
	
	Recognizer();
	
	double handVel; // hand velocity (手勢移動速度)
	GesName recognize(CvPoint hand);
	
	
private:

	CvPoint hand; // (最新的手勢位置)
	ptVectorWithCounter handBuf; // (手勢位置暫存器)
	
	int featureNum;
	vector<Feature> featureBuf; // (手勢特徵暫存器)
		
	Segmenter segmenter; // (特徵區段擷取器)
	Spotter spotter;     // (特徵區段辨識器)	
		
	void getFeature();		
	Feature getFeaVector(CvPoint p1, CvPoint p2);		
	
};






#endif