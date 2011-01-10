#ifndef __TRACKER_H__
#define __TRACKER_H__

#include <vector>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
using namespace std;

const double MOTION_TH = 0.4;
const int CC_SIZE_TH = 500;
const int CONNECT_COM_NUM = 10;
const int CLUSTER_BUF_LENGTH = 12;
const double CC_MERGR_TH = 50;
const double CLUSTER_MERGE_TH = 50;
const int HAND_BUF_LENGTH = 12;
const int REMOVE_CC_TH = 1500;

typedef vector<CvPoint> PointArr;
typedef vector< vector<CvPoint> > PointIn2DArr;
typedef vector<CvRect> RectArr;


class Tracker
{
public:

	Tracker(CvSize sz, int handNum); 
			
	IplImage* frameSkin;	
	IplImage* smoothSkin;

	PointArr currentHand;

	double avgGrayValue;
	double avgMaxCCArea;

	void getHands(IplImage* frame);	
	void updatePara(int area, int vel, double brightness);	
	void setTrackNum(int n);
	void unload();

private:

	int handNum;

	bool frameGrayBufferEmpty;
	bool enhanceMotionSkin;
	
	int adptHandAvgLength;
	int adptClusterCheckLength;
	double adptCluMergTh;
	double adptCCMergTh;	
	int adptRemoveCCTh;
	int adptResampleMargin;
	double adptMotionTh;
	
		
	IplImage* frameGray;		
	IplImage* frameGrayDiff;
	IplImage* frameGrayBuffer;
	IplImage* frameMotionSkin;
	IplImage* skinResample;	
	
	
	PointIn2DArr clusteredCenters;	
	PointIn2DArr handBuffer;
	vector<int> maxCCAreaBuf;
			
	double getAvgGrayValue(const IplImage* src);
	void frameDiff(const IplImage* cur, const IplImage* pre, IplImage* diff);
	void connectedCom(IplImage* input, float cc_permi, 
		              int* componentNum, RectArr &rects, PointArr &centers);
	void getCandidate(const IplImage* skin, IplImage* skinMotion, 
		              int* ccNumFinal, RectArr &rectsFinal, PointArr &centersFinal);		
	void clusterCandidate(const PointArr &centersFinal);
	void getEmptyIndex(const PointIn2DArr &massbuffer, vector<bool> &emptyIndex);	
	void getHandFromCandidate();	
	int  getCenterBufIndex(CvPoint mass, const PointIn2DArr &buf, 
		int checkLength, const vector<bool> &emptyIndex);
	void removeSmallCandidates(int* ccNum, RectArr &rects, PointArr &centers);
	void updateAvgMaxCCRect(RectArr &rects);

	
	
};

#endif