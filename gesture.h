
#ifndef __GESTURE_H__
#define __GESTURE_H__

#include <vector>
#include <iostream>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "enumList.h"

using namespace std;

enum GesName;

// kernel class for exporting (供輸出動態連結庫的類)
class GesKernel {
public:
	
	GesKernel(int handNum);	

	vector<GesName> outGestures; // 輸出手勢, 向量大小依據追蹤手勢數量

	void run();	// 主要的辨識演算法流程

	virtual void react() {} // 類使用者自定義之行為函式

	void switcherEnable()  { this->switcher = true; }
	void switcherDisable() { this->switcher = false; }
	bool isSwitcherOn() { return this->switcher; }

	void setCounter(int val) { this->counter = val; }
	int  getCounter() { return this->counter; }
	void increaseCounter() { 
		if (this->counter < 10000) 
			this->counter++;
		else
			this->counter = 0;		
	}

	void setGesBuffer (GesName name) { this->gesBuffer = name; }
	GesName getGesBuffer () { return this->gesBuffer; }
				
private:

	int handNum;

	bool switcher;	
	int counter;
	GesName gesBuffer;


	IplImage* smallFrameCopy;
	IplImage* bigFrameCopy;

	int initAll();
	void exitProgram(int code);
	void exitProgram();
	void captureVideoFrame();

};

#endif
