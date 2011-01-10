#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <vector>
#include <iostream>
#include <string>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

using namespace std;

typedef vector<CvPoint> PointArr;
typedef vector< vector<CvPoint> > PointIn2DArr;
typedef vector<CvRect> RectArr;

inline double getPointDistant(const CvPoint point1, const CvPoint point2)
{
	// Get the distant of two CvPoints
	return sqrt( (double)( (point1.x-point2.x)*(point1.x-point2.x) + 
		(point1.y-point2.y)*(point1.y-point2.y)  ) );
}
inline void convertCvPointVectorToArray(const PointArr &CvPointVector, CvPoint* CvPointArray)
{
	for (unsigned int i = 0; i < CvPointVector.size(); ++i)
		CvPointArray[i] = CvPointVector[i];	
}
inline void shiftCvPointBuffer(PointArr &cvPointBuffer, CvPoint newPoint) 
{
	for (int i = (cvPointBuffer.size()-2); i >= 0; i--) 
		cvPointBuffer[i+1] = cvPointBuffer[i];

	cvPointBuffer[0] = newPoint;	

}

template <class myType>
inline void shiftVector(vector<myType> &vec, myType element)
{
	for (int i = (vec.size()-2); i >= 0; i--) 
		vec[i+1] = vec[i];

	vec[0] = element;
}

void connectComponent(IplImage* inputframe, const int poly_hull0, const float perimScale, 
					  int *num, RectArr &rects, PointArr &centers);

void drawCC(IplImage* targetFrame, const int num, const RectArr &rects, const PointArr &centers);

void resampleByPoints(const IplImage* input, const int srMar, const PointArr &points, 
					  IplImage* output);

void kMeans(const PointArr &dataVector, const int clusterCount, PointIn2DArr &clusterContainer);

void packClusters(int clusterCount, const CvMat* points, CvMat* clusters, PointIn2DArr &clusterContainer);

void removeEmptyClusters(PointIn2DArr &clusterContainer);

void getClusterCenters(PointIn2DArr &points2DArr, PointArr &centers);

int mergeClusters(PointArr &clusterCenters, PointIn2DArr &clusterPoints, double MERGE_TH);

CvPoint findMostSimilar(PointArr &clusterCenters, double MERGE_TH, bool* mergeDone);

void merging(CvPoint toFrom, PointArr &clusterCenters, PointIn2DArr &clusterPoints);

int clustering(const PointArr &points, int clusterNum, double mergeRange,
			   PointIn2DArr &clusterContainer, PointArr &clusterMass);



void refineSkinArea(const IplImage* skin, 
					int clusterNum, PointArr &points, 
					IplImage* output, int* num, RectArr &rects, PointArr &centers);


CvPoint avgCvPoints(const PointArr &points, int num);

void drawHands(IplImage* targetFrame, const PointArr &currentHand, int num, CvSize sz);

void drawHandsBig(IplImage* targetFrame, const PointArr &currentHand, int num, CvSize sz);

void showMaskPart(const IplImage* src, const IplImage* mask, IplImage* result);

void maskByRects(const IplImage* src, const RectArr &rects, IplImage* result);

int getMaxRect(RectArr &rects);


inline void resetPointIn2DArr(PointIn2DArr &p2D)
{
	for (unsigned int i = 0; i < p2D.size(); ++i) 
		for (unsigned int j = 0; j < p2D[0].size(); ++j)
			p2D[i][j] = cvPoint(-1, -1);

}

inline void resetPointArr(PointArr &p)
{
	for (unsigned int i = 0; i < p.size(); ++i)
		p[i] = cvPoint(-1, -1);
}

template <class myType>
inline string num2str(myType &i)
{
	string s;
	stringstream ss(s);
	ss << i;

	return ss.str();
}

template <class myType>
inline void attachText(IplImage* img, string prefix, myType para, int x, int y, float size = 0.5)
{
	string out = prefix + num2str<myType>(para);
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, size, size, 0, 2, CV_AA);
	cvPutText(img, out.c_str(), cvPoint(x, y), &font, cvScalar(0, 0, 255, 0));
}
inline void attachText1(IplImage* img, string prefix, int x, int y, float size = 0.5)
{
	string out = prefix;
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, size, size, 0, 2, CV_AA);
	cvPutText(img, out.c_str(), cvPoint(x, y), &font, cvScalar(0, 0, 255, 0));
}



#endif
