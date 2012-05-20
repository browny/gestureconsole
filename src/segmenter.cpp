
#include "segmenter.h"

void Segmenter::intplToEqualLength(const vector<Feature> &inVec, vector<Feature> &eqlVec)
{
	/* 
	 * Pre : a Feature vector of varied length 
	 * Post: a Feature vector of fixed length (FEATURE_LENGTH)
	 */

	int len = inVec.size(); // 原始特徵向量長度
	Feature eqlFeature = {0, 0};

	for (int i = 0; i < FEATURE_LENGTH; ++i) {

		double intepPoint = static_cast<double>(i) * 
			(static_cast<double>(len-1) / static_cast<double>(FEATURE_LENGTH-1));

		double upBound_d  = ceil(intepPoint);
		double lowBound_d = floor(intepPoint);

		if (upBound_d > static_cast<double>((len-1)))
			upBound_d = static_cast<double>((len-1));

		if (lowBound_d > static_cast<double>((len-1)))
			lowBound_d = static_cast<double>((len-1));

		double x = upBound_d - lowBound_d;
		if (upBound_d == lowBound_d) 
			x = 1;

		double y;
		double intepValue;

		int upBound  = static_cast<int>(upBound_d);
		int lowBound = static_cast<int>(lowBound_d);		

		// sin & cos
		y = inVec[upBound].sin - inVec[lowBound].sin;
		intepValue = inVec[lowBound].sin + (intepPoint-lowBound)*(y/x);
		eqlFeature.sin = intepValue;

		y = inVec[upBound].cos - inVec[lowBound].cos;			
		intepValue = inVec[lowBound].cos + (intepPoint-lowBound)*(y/x);
		eqlFeature.cos = intepValue;

		eqlVec[i] = eqlFeature;

	}		
}
void Segmenter::convertToArray(const vector<Feature> &eqlVec, double* formatedFea)
{
	for (int i = 0; i < FEATURE_LENGTH; ++i) {

		int j = 0;

		formatedFea[i*FEATURE_DIM + j] = eqlVec[i].sin;
		j++;

		formatedFea[i*FEATURE_DIM + j] = eqlVec[i].cos;
		j++;

	}	
}
GesName Segmenter::outputGesture(const double* gestureScore)
{
	/*
	 * Pre : a gestureScore array which stores each gesture score calculated by SVM 
	 * Post: the highest score and bigger than threshold gesture 
	*/

	int highestScoreGes = 0;				
	for (int i = 0; i < GESTURE_NUM; ++i)				
		if (gestureScore[i] > gestureScore[highestScoreGes])
			highestScoreGes = i;						

	int finalGes;
	if (gestureScore[highestScoreGes] > GESTURE_SCORE_TH)
		finalGes = highestScoreGes;		
	else
		finalGes = GESTURE_NUM;	

	GesName outputGes;
	if (finalGes == 0) outputGes = Up;
	if (finalGes == 1) outputGes = Down;
	if (finalGes == 2) outputGes = Left;
	if (finalGes == 3) outputGes = Right;
	if (finalGes == 4) outputGes = Clock;
	if (finalGes == 5) outputGes = UnClock;
	if (finalGes == 6) outputGes = Wrong;
	if (finalGes == 7) outputGes = None;

	return outputGes;
}

vector<GesName> Segmenter::segment(int featureNum, const vector<Feature> &featureBuf)
{
	/* 
	 * Pre : "featureBuf" with enough features
	 * Post: A vector of all gesture possibility, first element is calculated from
	 *       current hand to (current hand - MIN_SEARCH_SIZE), ..., last element is
	 *       calculated from current hand to (current hand - MAX_SEARCH_SIZE).
	 *
	 *       SPOTTER_STEP_SIZE reduces the calculation from (MAX_SEARCH_SIZE - 
	 *       MIN_SEARCH_SIZE) to (MAX_SEARCH_TH - MIN_SEARCH_TH)/SPOTTER_STEP_SIZE + 1
	 */

	vector<GesName> watchList(MAX_SEARCH_TH-MIN_SEARCH_TH/SPOTTER_STEP_SIZE + 1, None);

	if ( (featureNum >= MIN_SEARCH_TH) ) { // enough features

		int nearestEnd = MIN_SEARCH_TH;	
		int farestEnd  = featureNum;
		
		if (featureNum > MAX_SEARCH_TH)
			farestEnd = MAX_SEARCH_TH;							

		int idx = 0;				
		for (int i = nearestEnd; i <= farestEnd; i+=SPOTTER_STEP_SIZE) {		

			vector<Feature>::const_iterator itBegin = featureBuf.begin();
			vector<Feature>::const_iterator itEnd = featureBuf.begin() + i;

			vector<Feature> segmentedFeatureBuf(itBegin, itEnd);
			reverse(segmentedFeatureBuf.begin(), segmentedFeatureBuf.end());			

			vector<Feature> featureBufEqlLength(FEATURE_LENGTH);
			intplToEqualLength(segmentedFeatureBuf, featureBufEqlLength);

			double formatedFea[FEATURE_DIM*FEATURE_LENGTH];
			convertToArray(featureBufEqlLength, formatedFea);

			double gestureScore[GESTURE_NUM];
			predict(formatedFea, FEATURE_DIM*FEATURE_LENGTH, gestureScore);	


			GesName ges = outputGesture(gestureScore);

			if (ges == Up)      watchList[idx] = Up;
			if (ges == Down)    watchList[idx] = Down;
			/*if (ges == Up)      watchList[idx] = None;
			if (ges == Down)    watchList[idx] = None;*/
			if (ges == Left)    watchList[idx] = Left;
			if (ges == Right)   watchList[idx] = Right;
			if (ges == Clock)   watchList[idx] = Clock;
			if (ges == UnClock) watchList[idx] = UnClock;
			if (ges == Wrong)   watchList[idx] = Wrong;
			if (ges == None)    watchList[idx] = None;	

			idx++;			

		}									
	}

	return watchList;
}