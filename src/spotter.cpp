
#include "spotter.h"

Spotter::Spotter()
{
	kindNum = 0;
	firCenter = 0;

	triggerCount = 0;
	gestureCandidate = None; 
	tempCenter = 0;
	
	banGes = None;
	
	firstGes = None;
	secGes   = None;
	outGes   = None;
		
	adaptTrigTh = TRIGGER_TH;

}

GesName Spotter::spotting(const vector<GesName> &watchList, double vel)
{
	kindNum   = howManyKindInList(watchList);	
	firCenter = centerOfList(watchList, firstGes);

	// for test (把watchList印出來)-------------------------------------
	/*
	string segGroup = "";
	for (int i = 0; i < watchList.size(); ++i) {
		
		if (watchList[i] == Up)      segGroup += "上 ";
		if (watchList[i] == Down)    segGroup += "下 ";
		if (watchList[i] == Left)    segGroup += "左 ";
		if (watchList[i] == Right)   segGroup += "右 ";
		if (watchList[i] == Clock)   segGroup += "順 ";
		if (watchList[i] == UnClock) segGroup += "逆 ";
		if (watchList[i] == Wrong)   segGroup += "○ ";
		if (watchList[i] == None)    segGroup += "○ ";
	}

	cout << " " << segGroup <<  kindNum << " cen:" << firCenter << 
	" " << firstGes << " " << secGes << endl;
	*/
	// ----------------------------------------------------	

	if (vel > 8)
		adaptTrigTh = 5;
	else
		adaptTrigTh = TRIGGER_TH;	


	if (kindNum == 1) { // one kind gesture in watchList (watchList當中只有一種手勢)

		if (triggerCount == 0) { // the first good gesture not happen yet

			if ( (firstGes != banGes) && (firCenter <= 1) ) { 	

				tempCenter = firCenter;
				gestureCandidate = firstGes;
				++triggerCount;

			}			


		} else {  

			if ( (firCenter >= tempCenter) && (firstGes == gestureCandidate) ) {
				// make sure the trend is moving right continuously
				
				tempCenter = firCenter;			
				gestureCandidate = firstGes;
				++triggerCount;

			} else {

				triggerCount = 0;
				gestureCandidate = None;				

			}
		}

	} else if (kindNum == 2) { // 2 kind gestures in watchList (watchList當中有兩種手勢)

		if (triggerCount == 0) { // the first good gesture not happen yet
			
			double firCenter = centerOfList(watchList, firstGes);
			
			if ( (firstGes != banGes) && (firCenter <= 1) ) { 														

				tempCenter = firCenter;
				gestureCandidate = firstGes;
				++triggerCount;


			} 
												
		} else {

			double secGesCenter = centerOfList(watchList, secGes);
			if ( (secGes == gestureCandidate) && (secGesCenter > tempCenter) ) {

				tempCenter = secGesCenter;	
				gestureCandidate = secGes;
				++triggerCount;

			} 

		}

	} else { // (watchList當中沒有手勢)

		triggerCount = 0;
		gestureCandidate = None;
		

	}
		
	outGes = None;
	if (triggerCount >= adaptTrigTh) {
				
		outGes = gestureCandidate;
		triggerCount = 0;			
		banGes = antiGes(outGes); 
				
	}

	return outGes;
	
}

int Spotter::howManyKindInList(vector<GesName> watchList)
{	
	/* 
	 * Pre : watchList(exclude "Wrong" and "None")
	 * Post: reture how many meaningful gesture (exclude "Wrong" and "None") included 
	 *       in "watchList" 
	 *       
	 *       write the first and second gestures to "firstGes" and "secGes"
	 */

	int num = 0;

	GesName ges = None;
	firstGes = None;
	secGes = None;


	for (unsigned int i = 0; i < watchList.size(); ++i) {
		GesName g = watchList[i];

		if ( (g != Wrong) && (g != None) ) {
			if (ges == None) {
				ges = g;				
				++num;
				firstGes = g;
			}
			else {
				if (g != ges) {
					ges = g;
					++num;
					if (num == 2)
						secGes = g;

				}
			}
		}		
	}

	if (ges == None)
		num = 0;

	return num;


}

double Spotter::centerOfList(vector<GesName> watchList, GesName targetGes)
{
	/*
	 * Pre : "watchList" are filled with various GesName.
	 * Post: return the center of "targetGes" in the watchList
	 */

	double cnt = 0;
	double sum = 0;

	for (unsigned int i = 0; i < watchList.size(); ++i) {
		
		GesName g = watchList[i];
		
		if (g == targetGes) {
			sum += i;
			++cnt;
		}				
	}

	double center = 0;
	if (cnt == 0)
		center = 0;
	else
		center = sum / cnt;
	

	return center;
}

GesName Spotter::antiGes(GesName ges)
{
	/*
	 * Pre : a gesture   
	 * Post: a anti gesture in sematics
	*/
	

	GesName anti = None;
	switch (ges)
	{
	case Up:
		anti = Down;
		break;
	case Down:
		anti = Up;
		break;
	case Left:
		anti = Right;
		break;
	case Right:
		anti = Left;
		break;
	default:
		break;
	}

	return anti;
}
