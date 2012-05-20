
///////////////////////////////////////////////////////
// Copyright (C) 2010 Browny Lin, shlin1983@gmail.com
///////////////////////////////////////////////////////

#include "gesture.h"

string g_gesString[4] = {"Up", "Down", "Left", "Right"};

class Gesture : public GesKernel
{
public:	

	Gesture(int p) : GesKernel(p) {} // p means how many hands you want to track

	// Define your reactions below 
	void react() {		
		for (unsigned int i = 0; i < outGestures.size(); ++i) {			
			if (outGestures[i] != 7) {
				int index = outGestures[i];
				/*cout << "第 " << (i+1) << " 個追蹤點的手勢: " << g_gesString[index] << endl;
				cout << endl;*/
			}
		}
	}
};

int main()
{		
	Gesture gesture(1);	
	gesture.run();
}
