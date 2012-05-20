#ifndef __SKIN_H__
#define __SKIN_H__

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
//#include "cvaux.h"

/*
#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <iterator>
using namespace std;


const int BLK_SIZE = 10;
const int GLOBAL_UPBOUND = 43;
const int GLOBAL_LOWBOUND = 3;
const double W = 0.03;
const double DIS_TO_BOUNDARY = 15;

const double MIN_LOW_BOUND = 3;
const double MAX_UP_BOUND  = 60;

const int MIN_BOUND_INTERVAL = 15;
*/

class SkinDetector
{
public:	

	SkinDetector();
		
	void detectSkin(const IplImage* frame, IplImage* frameSkin);

	void unload();
	
protected:
private:

	IplImage* curr;
	
	/*
	int adptHueLowBound;
	int adptHueUpBound;

	CvSize sz;
	IplImage* hsv_image;
	IplImage* H_image;	
	IplImage* globalSkinResult;
	IplImage* updatedPartialSkin;
	IplImage* updatedPartialSkinRGB;
	IplImage* updatedPartialSkinMask;
	IplImage* imgHistogram;
	
	CvScalar getUpdatedBoundary(const vector<int>& hist);
	SkinDetector(CvSize sz);	
	void globalSkinDetect(const IplImage* frame, IplImage* frameSkin);
	void calcMaskImgHist(const int srMar, const int pointsNum, 
		const vector<CvPoint>& points);

	void updateHueRange(int center);

	void getTrackerSkinInfo(const int pointNum, const vector<CvPoint>& points);

	void adptSkinDetect(const IplImage* frame, IplImage* frameSkin);

	void unload();
	*/
	
	
};

#endif