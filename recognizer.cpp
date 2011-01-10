
#include "recognizer.h"

ptVectorWithCounter::ptVectorWithCounter()
{
	data.resize(0);
	count = 0;
}

void ptVectorWithCounter::addPoint(CvPoint point)
{
	if (point.x != -1) { 
		// if the hand is detected, push it into buffer
		for (int i = ( data.size()-2 ); i >= 0; --i)
			data[i+1] = data[i];

		data[0] = point;

		count++;

	} else {
		// if the hand not detected, reset buffer
		data.resize(data.size(), cvPoint(0,0));
		count = 0;		
		
	}		

}

Recognizer::Recognizer()
{
	this->hand = cvPoint(-1, -1);	
	handBuf.data.resize(MAX_SEARCH_TH+1);
	handBuf.count = 0;

	featureBuf = vector<Feature>(MAX_SEARCH_TH);		
	featureNum = 0;

	handVel = 0;	
	
}

GesName Recognizer::recognize(CvPoint hand)
{
	this->hand = hand;	
	
	handBuf.addPoint(this->hand);
	getFeature();		
	
	// (將手勢特徵暫存器中的特徵分段切割, 並將每一段特徵串輸出一個手勢名稱,
	//  存放至 vector<GesName> 當中)	
	vector<GesName> watchList = segmenter.segment(this->featureNum, this->featureBuf);	

	if (this->hand.x == -1)
		spotter.banGes = None;
	
	GesName outputGes = spotter.spotting(watchList, this->handVel);
	
	return outputGes;
			
}

void Recognizer::getFeature()
{	
	/* Pre:  handBuf
	 * Post: transform the points in "handBuf" into the features in "featureBuf"
	 */

	if (this->handBuf.count >= 2) { // make sure we have >= 2 locations to calculate vector

		CvPoint currentHand  = this->handBuf.data[0];
		CvPoint previousHand = this->handBuf.data[1];
		
		Feature fea = getFeaVector(currentHand, previousHand);		
		
		// add fea to featureBuf
		shiftVector<Feature>(this->featureBuf, fea);
		featureNum++;

		this->handVel = fea.vel;
				

	} else {

		// clear featureBuf
		Feature fea = {0, 0, 0};		
		featureBuf.resize(MAX_SEARCH_TH, fea);
		featureNum = 0;

		this->handVel = fea.vel;
		
	}
	
}




Feature Recognizer::getFeaVector(CvPoint p1, CvPoint p2)
{
	/* Pre: given 2 points 
	 * Post: sin, cos and velocity features
	 */

	double x = p1.x - p2.x;
	double y = p1.y - p2.y;
	
	double dis = sqrt((double)((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)));
	
	Feature fea = {0, 0, 0};

	if (dis != 0) {
		fea.sin = y / dis;
		fea.cos = x / dis;
		fea.vel = dis;
	}	
	
	return fea;
}


