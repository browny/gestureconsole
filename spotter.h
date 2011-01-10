
#ifndef __SPOTTER_H__
#define __SPOTTER_H__

#include <vector>
#include <iostream>
#include <string>
#include "enumList.h"
using namespace std;

const int TRIGGER_TH = 6;

class Spotter
{
public:

	Spotter();
	
	GesName outGes;
	GesName banGes;
	int adaptTrigTh;
		
	GesName spotting(const vector<GesName> &watchList, double vel);
	

private:
	int kindNum; // there is "kindNum" meaningful gestures in "watchList"
	double firCenter; 
	GesName firstGes;
	GesName secGes;
	
		
	int triggerCount;
	GesName gestureCandidate;
	double tempCenter;

	int howManyKindInList(vector<GesName> watchList);
	double centerOfList(vector<GesName> watchList, GesName targetGes);
	GesName antiGes(GesName ges);
	

};


#endif