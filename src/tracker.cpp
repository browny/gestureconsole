
#include <iostream>
#include <numeric>
#include "tracker.h"
#include "utility.h"
using namespace std;


Tracker::Tracker(CvSize sz, int handNum)
{	
	this->handNum = handNum;

	adptHandAvgLength      = 6;
	adptClusterCheckLength = 6;
	adptCluMergTh          = CLUSTER_MERGE_TH;
	adptCCMergTh           = CC_MERGR_TH;
	adptRemoveCCTh         = REMOVE_CC_TH;
	adptMotionTh           = MOTION_TH;
	adptResampleMargin     = 15;
	
	avgGrayValue = 0;
	avgMaxCCArea = 0;
	
	frameGrayBufferEmpty = true;
	enhanceMotionSkin = false;
	frameGray       = cvCreateImage(sz, 8, 1);
	frameSkin       = cvCreateImage(sz, 8, 1);	
	frameGrayDiff   = cvCreateImage(sz, 8, 1);
	frameGrayBuffer = cvCreateImage(sz, 8, 1);
	frameMotionSkin = cvCreateImage(sz, 8, 1);
	skinResample    = cvCreateImage(sz, 8, 1);	
	smoothSkin      = cvCreateImage(sz, 8, 1);
	
	//PointArr temp(CLUSTER_BUF_LENGTH);
	clusteredCenters = PointIn2DArr(this->handNum, PointArr(CLUSTER_BUF_LENGTH));
	resetPointIn2DArr(clusteredCenters);
			
	handBuffer = PointIn2DArr(this->handNum, PointArr(HAND_BUF_LENGTH));
	resetPointIn2DArr(handBuffer);
	
	currentHand = PointArr(this->handNum);
	resetPointArr(currentHand);	

	maxCCAreaBuf = vector<int>(30, 0);

}
void Tracker::setTrackNum(int n)
{
	this->handNum = n;
}
void Tracker::getHands(IplImage* frame) 
{	
	cvCvtColor(frame, frameGray, CV_BGR2GRAY);

	if ( frameGrayBufferEmpty == false ) {
					
		// Generate motion-skin frame	
		this->avgGrayValue = getAvgGrayValue(frameGray);		

		frameDiff(frameGray, frameGrayBuffer, frameGrayDiff);
		cvAnd(frameGrayDiff, smoothSkin, frameMotionSkin, 0);

		// If frame is too dark, enhance its motion
		if ( enhanceMotionSkin ) {
			cvDilate(frameMotionSkin, frameMotionSkin);				
			//cout << "Enhance" << endl;
		} else {
			//cout << "NONO" << endl;
		}
		
#ifdef __TEST__
		cvShowImage("motionSkin", frameMotionSkin);
#endif
							
		// Generate candidate hands
		int ccNum = CONNECT_COM_NUM;	
		RectArr rects(0);
		PointArr centers(0);
		
		getCandidate(smoothSkin, frameMotionSkin, &ccNum, rects, centers);		
		updateAvgMaxCCRect(rects); // to know how far the tracking hand away from webcam
		removeSmallCandidates(&ccNum, rects, centers);	
		//drawCC(frame, ccNum, rects, centers);	

		// Get final hands
		clusterCandidate(centers);		
		getHandFromCandidate();		
		drawHands(frame, currentHand, this->handNum, cvGetSize(frame));						
		
	}

	if (frameGrayBufferEmpty == true)
		frameGrayBufferEmpty = false;

	cvCopy(frameGray, frameGrayBuffer);		
	
}
double Tracker::getAvgGrayValue(const IplImage* src)
{
	CvScalar avgGrayValue = cvAvg(src, 0);
	return avgGrayValue.val[0];
}
void Tracker::frameDiff(const IplImage* cur, const IplImage* pre, IplImage* diff) 
{
	// Calculate the frame differences between "cur" and "pre", and threshold it by 
	// a adptive threshold MOTION_TH*(avgGrayValue). Reture the motion part to "diff"

	cvAbsDiff(cur, pre, diff);	
	CvScalar avgGrayValue = cvAvg(cur, 0);	

	this->adptMotionTh = MOTION_TH * avgGrayValue.val[0];
		
	if (this->avgGrayValue < 50)
		this->adptMotionTh = 40; // for dark
			
	cvThreshold(diff, diff, this->adptMotionTh, 255, CV_THRESH_BINARY);

}

void Tracker::connectedCom(IplImage* input, float cc_permi, 
						   int* componentNum, RectArr &rects, PointArr &centers)
{
	connectComponent(input, 1, cc_permi, componentNum, rects, centers);	
}

void Tracker::getCandidate(const IplImage* skin, IplImage* skinMotion, 
						   int* ccNum, RectArr &rects, PointArr &centers)
{	
	// Connected component	
	connectedCom(skinMotion, 5000.0f, ccNum, rects, centers);	
	
		
	// Re-sample frameSkin	
	resampleByPoints(skin, adptResampleMargin, centers, skinResample);	

#ifdef __TEST__
	cvShowImage("resampleSkin", skinResample);
#endif
			
	// Connected component again	
	*ccNum = CONNECT_COM_NUM;
	rects.clear();
	centers.clear();
	connectedCom(skinResample, (float)CC_SIZE_TH, ccNum, rects, centers);		

	// Refine
	//refineSkinArea(skin, CLUSTER_NUM, centers, cadidate, ccNumFinal, rectsFinal, centersFinal);		
	
}
void Tracker::removeSmallCandidates(int* ccNum, RectArr &rects, PointArr &centers)
{
	if (this->avgMaxCCArea > 2000) {
		// only the tracking hand is near webcam, I do filtering
		// otherwise, I do nothing

		RectArr::iterator it = rects.begin();
		PointArr::iterator it2 = centers.begin();	

		while (it != rects.end())
		{
			if ( (it->width * it->height) < adptRemoveCCTh ) {

				rects.erase(it);
				centers.erase(it2);
				(*ccNum)--;			

				it  = rects.begin();
				it2 = centers.begin();

			} else {			

				it++;
				it2++;
			}				
		}	

	}
	
}

void Tracker::clusterCandidate(const PointArr &centers)
{			

	for (int i = 0; i < this->handNum; ++i) {				
		shiftVector<CvPoint>(this->clusteredCenters[i], cvPoint(-1,-1));
	}

	PointArr mass;				
	PointIn2DArr points;
	
	if (centers.size() > 0) {
				
		clustering(centers, this->handNum, adptCCMergTh, points, mass);	

		// Judge if all clusteredCenters were empty
		vector<bool> empty(this->handNum, true);		
		getEmptyIndex(this->clusteredCenters, empty);		

		bool allBufferIsEmpty = true;
		for (int i = 0; i < this->handNum; ++i)
			if (empty[i] == false)
				allBufferIsEmpty = false;	

		if (allBufferIsEmpty) { // all clusteredCenters are empty
			
			for (unsigned int i = 0; i < mass.size(); ++i) 
				this->clusteredCenters[i][0] = mass[i]; // put the CandiMass into buffer
			
		} else {

			for (unsigned int i = 0; i < mass.size(); ++i) {
				
				CvPoint tempMass = mass[i];
				PointArr tempPoint = points[i];

				int idx = getCenterBufIndex(tempMass, this->clusteredCenters, 
					adptClusterCheckLength, empty);
				
				if (idx != -1) {

					// Find one cluster to be put into
					this->clusteredCenters[idx][0] = tempMass;
					

				} else { 

					// The current CandiMass can't find any cluster to be put into. Find
					// if there were any empty buffer to be first member of new cluster
					for (int j = 0; j < this->handNum; ++j) {
						if (empty[j] == true) {														

							this->clusteredCenters[j][0] = tempMass;
							
							break;

						}
					}
				}
			}
		}				
	}
	

}
void Tracker::getEmptyIndex(const PointIn2DArr &massbuffer, vector<bool> &empty) 
{	
	// Judge each clusterCandiMassBuffer is empty or non-empty

	for (int i = 0; i < this->handNum; ++i) {

		empty[i] = true;

		for (unsigned int j = 0; j < massbuffer[i].size(); ++j) 			
			if (massbuffer[i][j].x != -1) 			
				empty[i] = false;

	}	
	
}


void Tracker::getHandFromCandidate()
{
	for (int i = 0; i < this->handNum; ++i) {
				
		shiftVector<CvPoint>(handBuffer[i], clusteredCenters[i][0]);
		
		// Average the hand buffer as the final detected hand
		CvPoint bufCenter = avgCvPoints(handBuffer[i], adptHandAvgLength);		

		currentHand[i] = bufCenter;

	}

	
}

void Tracker::unload()
{
	cvReleaseImage(&frameGray);
	cvReleaseImage(&frameSkin);	
	cvReleaseImage(&frameGrayDiff);
	cvReleaseImage(&frameGrayBuffer);
	cvReleaseImage(&frameMotionSkin);	
	cvReleaseImage(&skinResample);
	cvReleaseImage(&smoothSkin);

}


int  Tracker::getCenterBufIndex(CvPoint mass, const PointIn2DArr &buf, int checkLength, const vector<bool> &empty)
{
	double minDis = 999999999; 
	int candiMassBufIndex = -1;

	for (int j = 0; j < this->handNum; ++j) {

		if (!empty[j]) {

			for (int k = 1; k < (1+checkLength); ++k) {

				if (buf[j][k].x != -1) {

					if ((getPointDistant(mass, buf[j][k]) < adptCluMergTh) && 
						(getPointDistant(mass, buf[j][k]) < minDis)) 
					{

						minDis = getPointDistant(mass, buf[j][k]);	
						candiMassBufIndex = j;					
					}										
				}
			}
		}

	}

	return candiMassBufIndex;

}


void Tracker::updatePara(int area, int vel, double brightness)
{
	if (vel != 0) {
		if (vel < 5)
			adptHandAvgLength = 7;
		else
			adptHandAvgLength = 6;
	}

	if (area != 0) {
		if (area < 2000) {

			adptCCMergTh  = CC_MERGR_TH;	
			adptCluMergTh = CLUSTER_MERGE_TH;
			adptRemoveCCTh = 0;
			adptResampleMargin = 15;

		} else {

			adptCCMergTh  = 2*CC_MERGR_TH;
			adptCluMergTh = 2*CLUSTER_MERGE_TH;
			adptRemoveCCTh = REMOVE_CC_TH;
			adptResampleMargin = 20;

		}

	}

	if (brightness < 50) {
		this->enhanceMotionSkin = true;
		
	} else {
		this->enhanceMotionSkin = false;
		
	}


}
void Tracker::updateAvgMaxCCRect(RectArr &rects)
{
	int maxCCArea = getMaxRect(rects);
	
	if (maxCCArea != 0) {			
		shiftVector<int>(this->maxCCAreaBuf, maxCCArea);
	}
	
	this->avgMaxCCArea = accumulate(this->maxCCAreaBuf.begin(), this->maxCCAreaBuf.end(), 0) /
		maxCCAreaBuf.size();
	
}