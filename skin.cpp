
#include "skin.h"
#include "skinAlgo.h"

SkinDetector::SkinDetector()
{
	curr = NULL;
}

void SkinDetector::detectSkin(const IplImage* frame, IplImage* frameSkin)
{	
	if (curr == NULL)
		curr = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, frame->nChannels);

	cvSkinDetect_Ellipse(frame, frameSkin, curr);
	
	/*cvErode(frameSkin, frameSkin);
	cvDilate(frameSkin, frameSkin);*/
}

void SkinDetector::unload()
{
	cvReleaseImage(&curr);
}

/*
SkinDetector::SkinDetector(CvSize sz)
{
	this->sz = sz;	
	hsv_image = cvCreateImage(this->sz, 8, 3);
	H_image = cvCreateImage(this->sz, 8, 1);
	
	updatedPartialSkin = cvCreateImage(this->sz, 8, 3);
	updatedPartialSkinRGB = cvCreateImage(this->sz, 8, 3);
	updatedPartialSkinMask = cvCreateImage(this->sz, 8, 1);

	imgHistogram = cvCreateImage(cvSize(256, 50), 8, 1);
	cvRectangle(imgHistogram, cvPoint(0,0),cvPoint(256,50),CV_RGB(255,255,255),-1);

	globalSkinResult = cvCreateImage(this->sz, 8, 1);

	adptHueLowBound = GLOBAL_LOWBOUND;
	adptHueUpBound  = GLOBAL_UPBOUND;
	
}


void SkinDetector::oldSkinDetect(const IplImage* frame, IplImage* frameSkin, int method) const
{	
	if (method == 0) {

		CvScalar  hsv_min   = cvScalar(0, 30, 80, 0);
		CvScalar  hsv_max   = cvScalar(20, 150, 255, 0);		
		cvCvtColor(frame, hsv_image, CV_BGR2HSV);
		cvInRangeS(hsv_image, hsv_min, hsv_max, frameSkin);			
		
	} 
	else if (method == 1) {

		IplImage* srcChB  = cvCreateImage(sz, 8, 1);
		IplImage* srcChG  = cvCreateImage(sz, 8, 1);
		IplImage* srcChR  = cvCreateImage(sz, 8, 1);
		IplImage* ChG_ChB = cvCreateImage(sz, 8, 1);
		IplImage* ChR_ChG = cvCreateImage(sz, 8, 1);

		cvSplit(frame, srcChB, srcChG, srcChR, NULL);

		cvSub(srcChG, srcChB, ChG_ChB);
		cvThreshold(ChG_ChB, ChG_ChB, 0, 255, CV_THRESH_BINARY);

		cvSub(srcChR, srcChG, ChR_ChG);
		cvThreshold(ChR_ChG, ChR_ChG, 0, 255, CV_THRESH_BINARY);

		cvAnd(ChR_ChG, ChG_ChB, frameSkin);

		cvReleaseImage(&srcChB);
		cvReleaseImage(&srcChG);
		cvReleaseImage(&srcChR);
		cvReleaseImage(&ChG_ChB);
		cvReleaseImage(&ChR_ChG);	

	} else if (method == 2) {
		
		cvSkinDetect_Ellipse(frame, frameSkin);				

	}

	

}
void SkinDetector::globalSkinDetect(const IplImage* frame, IplImage* frameSkin)
{
	cvCvtColor(frame, hsv_image, CV_BGR2HSV);
	cvSplit(hsv_image, H_image, NULL, NULL, NULL);
	CvScalar lowBound = cvScalar(3);
	CvScalar upBound = cvScalar(43);
	cvInRangeS(H_image, lowBound, upBound, frameSkin);
}

void SkinDetector::calcMaskImgHist(const int srMar, const int pointsNum, 
								   const vector<CvPoint>& points)
{

	vector<int> hist(256, 0);
	CvSize sz = cvSize(H_image->width, H_image->height);
	cvZero(updatedPartialSkinMask);
	
	for (int i = 0; i < pointsNum; ++i) {

		CvPoint leftTop = cvPoint(points[i].x - srMar, points[i].y - srMar);
		CvPoint rightBottom = cvPoint(points[i].x + srMar, points[i].y + srMar);

		if (leftTop.x < 0)
			leftTop.x = 0;

		if (leftTop.y < 0)
			leftTop.y = 0;

		if (rightBottom.x > sz.width)
			rightBottom.x = sz.width;

		if (rightBottom.y > sz.height)
			rightBottom.y = sz.height;

		for (int j = leftTop.y; j < rightBottom.y; j++) {

			uchar* ptr = (uchar*)(H_image->imageData + j*H_image->widthStep);
			uchar* pMask = (uchar*)(updatedPartialSkinMask->imageData + 
				                    j*updatedPartialSkinMask->widthStep);

			for (int k = leftTop.x; k < rightBottom.x; k++) {
				int pixelValue = ptr[k];
				hist[pixelValue]++;

				pMask[k] = 255;
				
				
			}

		}
	}
	
	cvRectangle(imgHistogram, cvPoint(0,0),cvPoint(256,50),CV_RGB(255,255,255),-1);
	int max_value = *(max_element(hist.begin(), hist.end()));
	for (int i = 0; i < 256; ++i) {
		int val = hist[i];
		int nor = cvRound(val * 50 / max_value);
		cvLine(imgHistogram, cvPoint(i, 50), cvPoint(i, 50-nor), CV_RGB(0,0,0));

	}

	cvShowImage("hist", imgHistogram);

	getUpdatedBoundary(hist);		
	

	int aa = 1;

}

void SkinDetector::updateHueRange(int center)
{	
	double cent = center;
	double lowBound = cent - DIS_TO_BOUNDARY;
	double upBound = cent + DIS_TO_BOUNDARY;

	if (lowBound < MIN_LOW_BOUND)
		adptHueLowBound = (1-W)*((double)adptHueLowBound) + W * MIN_LOW_BOUND;	
	else
		adptHueLowBound = (1-W)*((double)adptHueLowBound) + W * lowBound;


	if (upBound > MAX_UP_BOUND)
		adptHueUpBound = (1-W)*((double)adptHueUpBound) + W*((double)MAX_UP_BOUND);	
	else
		adptHueUpBound = (1-W)*((double)adptHueUpBound) + W * upBound;

	if ( (adptHueUpBound - adptHueLowBound) < MIN_BOUND_INTERVAL ) {
		adptHueLowBound = GLOBAL_LOWBOUND;
		adptHueUpBound  = GLOBAL_UPBOUND;
	}
	
}

void SkinDetector::getTrackerSkinInfo(const int pointNum, const vector<CvPoint>& points)
{
	if (pointNum != 0) {
		calcMaskImgHist(BLK_SIZE, pointNum, points);
	}
}

void SkinDetector::adptSkinDetect(const IplImage* frame, IplImage* frameSkin)
{
	cvCvtColor(frame, hsv_image, CV_BGR2HSV);
	cvSplit(hsv_image, H_image, NULL, NULL, NULL);	
	cvInRangeS(H_image, cvScalar(adptHueLowBound), cvScalar(adptHueUpBound), frameSkin);
}


CvScalar SkinDetector::getUpdatedBoundary(const vector<int>& hist)
{
	CvScalar updatedBound = cvScalar(0, 0);

	double sum = 0;
	int center = 0;

	for (int i = 0; i < hist.size(); ++i)
		sum += i*hist[i];

	double deno = accumulate(hist.begin(), hist.end(), 0);
	center = sum / deno;

	updateHueRange(center);

	cout << "low: " << adptHueLowBound << " up: " << adptHueUpBound << endl;
	cout << "cent: " << center << endl;

	return updatedBound;
		

}

void SkinDetector::unload()
{
	cvReleaseImage(&hsv_image);
	cvReleaseImage(&H_image);
	cvReleaseImage(&updatedPartialSkin);
	cvReleaseImage(&updatedPartialSkinRGB);
	cvReleaseImage(&updatedPartialSkinMask);

	cvReleaseImage(&imgHistogram);

	cvReleaseImage(&globalSkinResult);
	
}

*/