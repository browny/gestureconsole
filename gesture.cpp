
#include <iostream>

#include "gesture.h"
#include "tracker.h"
#include "recognizer.h"
#include "reactor.h"
#include "skin.h"
#include "svm-predict.h"
#include "Timer.h"
#include "capture.h"
#include "enumList.h"


//#define __TEST__

const char* DISPLAY_WINDOW = "GestureDemo";
const CvSize PROCESS_SIZE = cvSize(320, 240);

int g_targApp = 0; // 0:PowerPoint, 1:EasyViewer, 2:MediaPlayer (控制應用程式的旗標)
bool g_bigScreen = false;
const int MAX_TRACKING_NUM = 3; // max hand tracking number (最大手勢追蹤數量)

bool g_readyToEnable = false;
const int INTERVAL_TH = 65; // 手勢要保持的最大間隔
const int EN_INTERVAL_TH = 35; // 要開啟開關, 連續兩個手勢必須小於這個間隔

GesKernel::GesKernel(int handNum)
{
	this->handNum = handNum;

	switcher = false;	
	counter = 0;
	gesBuffer = None;

	smallFrameCopy = cvCreateImage(PROCESS_SIZE, 8, 3);	
	bigFrameCopy = cvCreateImage(cvSize(2*PROCESS_SIZE.width, 2*PROCESS_SIZE.height), 8, 3);
	outGestures.resize(handNum);
}

void GesKernel::run()
{
	if( !initAll() ) 
		exitProgram(EXIT_FAILURE);	

	SkinDetector skinDetector;
	Tracker tracker(PROCESS_SIZE, MAX_TRACKING_NUM); 
	Recognizer* recognizer = new Recognizer[MAX_TRACKING_NUM];	
	Reactor* reactor = new Reactor[MAX_TRACKING_NUM];		

	tracker.setTrackNum(handNum);

	bool loopRun = true;
	while(loopRun) {		

		captureVideoFrame();			

		// ---- Start Processing (開始) ----	
		Timer timer;
		timer.start();



		// Skin Detection (膚色偵測)				
		skinDetector.detectSkin(smallFrameCopy, tracker.frameSkin);		
		cvSmooth(tracker.frameSkin, tracker.smoothSkin, CV_GAUSSIAN, 5, 5);

		#ifdef __TEST__		
		cvShowImage("smooth skin", tracker.smoothSkin);
		#endif
		
		// Hand detection (手部追蹤)
		tracker.getHands(smallFrameCopy); 	
		if (g_bigScreen)
			drawHandsBig(bigFrameCopy, tracker.currentHand, handNum, cvGetSize(bigFrameCopy));		
		
		// Gesture recognize (路徑辨識)
		double maxVel = 0;
		increaseCounter();
		for (int i = 0; i < this->handNum; ++i) {		

			GesName ges = recognizer[i].recognize(tracker.currentHand[i]);	

			if (isSwitcherOn()) {		

				if (getCounter() > INTERVAL_TH) { 
					switcherDisable(); 
					g_readyToEnable = false;
					//cout << "switch disable: " << endl;

				}

				if (ges != None) {

					setCounter(0);

				} 

			} else { // 開關尚未打開

				if (getCounter() > EN_INTERVAL_TH) { 					 
					setGesBuffer(None);
					g_readyToEnable = false;
					
				}

				if (ges != None) {

					if (getGesBuffer() == None) { // buffer為空, 進來一個手勢

						setGesBuffer(ges);
						g_readyToEnable = true;
						

					} else {  // buffer非空, 進來一個手勢

						if (getGesBuffer() == ges) { // 進來的手勢和buffer的手勢為同一種

							switcherEnable();
							g_readyToEnable = false;
							//cout << "switch enable: " << getCounter() << endl;

						} 						

					}

					setCounter(0);
				}			
			}							

			// output the velocity parameter
			if (recognizer[i].handVel > maxVel)
				maxVel = recognizer[i].handVel;

			// 針對偵測到的手勢, 產生對應的介面控制
			if (isSwitcherOn()) {
				reactor[i].react(ges, g_targApp);
				this->outGestures[i] = ges;
			}
		}

		this->react(); // class user defined method (類別使用者自定義的反應方法)

		// Update tracker parameters (更新手勢追蹤適應性參數)
		tracker.updatePara( (int)tracker.avgMaxCCArea, (int)maxVel, tracker.avgGrayValue );

		timer.stop();
		//int fps = (int)(1 / timer.getElapsedTimeInSec());

		// ---- End Processing (結束) ----	

		// 印出開關是否開啟
		if (g_bigScreen) {

			if (isSwitcherOn()) {
				cvCircle(bigFrameCopy, cvPoint((PROCESS_SIZE.width-20)*2, (PROCESS_SIZE.height-20)*2), 
				20, CV_RGB(0,255,0), -2);								

			}
			else if (!isSwitcherOn())
				cvCircle(bigFrameCopy, cvPoint((PROCESS_SIZE.width-20)*2, (PROCESS_SIZE.height-20)*2), 
				20, CV_RGB(255,0,0), -2);

			if (g_readyToEnable)
				cvCircle(bigFrameCopy, cvPoint((PROCESS_SIZE.width-20)*2, (PROCESS_SIZE.height-20)*2), 
				20, CV_RGB(255,255,0), -2);

		} else {
			if (isSwitcherOn()) {
				cvCircle(smallFrameCopy, cvPoint(PROCESS_SIZE.width-20, PROCESS_SIZE.height-20), 
				10, CV_RGB(0,255,0), -2);

			}
			else if (!isSwitcherOn())
				cvCircle(smallFrameCopy, cvPoint(PROCESS_SIZE.width-20, PROCESS_SIZE.height-20), 
				10, CV_RGB(255,0,0), -2);

			if (g_readyToEnable)
				cvCircle(smallFrameCopy, cvPoint(PROCESS_SIZE.width-20, PROCESS_SIZE.height-20), 
				10, CV_RGB(255,255,0), -2);
		}
		
	
		// Display hand tracking nume (顯示手勢追蹤數量)
		if (g_bigScreen)
			attachText(bigFrameCopy, "Num: ", this->handNum, 2*PROCESS_SIZE.width-70, 15);
		else
			attachText(smallFrameCopy, "Num: ", this->handNum, PROCESS_SIZE.width-70, 15);		

        // Display targeted app (顯示當前受控制的程式)

		switch (g_targApp)
		{
		case 0:
			if (g_bigScreen)
				attachText1(bigFrameCopy, "PPT", 10, 2*PROCESS_SIZE.height-10, 0.7f);
			else
				attachText1(smallFrameCopy, "PPT", 10, PROCESS_SIZE.height-10, 0.7f);
			
			break;
		case 1:
			if (g_bigScreen)
				attachText1(bigFrameCopy, "EV", 10, 2*PROCESS_SIZE.height-10, 0.7f);
			else
				attachText1(smallFrameCopy, "EV", 10, PROCESS_SIZE.height-10, 0.7f);

			break;
		case 2:
			if (g_bigScreen)
				attachText1(bigFrameCopy, "Media", 10, 2*PROCESS_SIZE.height-10, 0.7f);
			else
				attachText1(smallFrameCopy, "Media", 10, PROCESS_SIZE.height-10, 0.7f);

			break;
		default:
			break;
		}	

		#ifdef __TEST__
		attachText(smallFrameCopy, "light: ", (int)tracker.avgGrayValue, 10, 15);
		attachText(smallFrameCopy, "area: ", tracker.avgMaxCCArea, 10, 35);
		attachText(smallFrameCopy, "vel: ", (int)maxVel, 10, 55);
		attachText(smallFrameCopy, "fps: ", fps, 10, 75);		
		#endif		
			
		if (g_bigScreen)
			cvShowImage( DISPLAY_WINDOW, bigFrameCopy );	
		else
			cvShowImage( DISPLAY_WINDOW, smallFrameCopy );			

		// (keyboard event 鍵盤事件)

		int c = cvWaitKey(30);
		switch (c)
		{
		case 27: // 'Esc'
			tracker.unload();	
			skinDetector.unload();
			svmUnload();

			exitProgram();
			loopRun = false;
			break;
		case 61: // '+'
			if (this->handNum < 3) {
				this->handNum++;
				tracker.setTrackNum(this->handNum);				
				outGestures.resize(this->handNum);
			}
			break;
		case 45: // '-'
			if (this->handNum > 1) {
				this->handNum--;
				tracker.setTrackNum(this->handNum);				
				outGestures.resize(this->handNum);
			}
			break;
		case 'p': // ppt
			g_targApp = 0;
			break;
		case 'e': // easy viewer
			g_targApp = 1;
			break;
		case 'm': // media player
			g_targApp = 2;
			break;
		case 'b':
			g_bigScreen = (g_bigScreen) ? false : true;			
			break;
		
		default:
			break;
		}		

	}
	
	delete [] recognizer;
	delete [] reactor;
}

int GesKernel::initAll()
{
	if( !initCapture(fromCam) ) 
		return 0;	
	
	cvNamedWindow( DISPLAY_WINDOW, 1 );
	
	return 1;
}

void GesKernel::exitProgram(int code)
{	
	closeCapture();	
	cvReleaseImage(&smallFrameCopy);
	cvReleaseImage(&bigFrameCopy);
	cvDestroyAllWindows();

	exit(code);
}

void GesKernel::exitProgram()
{	
	closeCapture();		
	cvReleaseImage(&smallFrameCopy);
	cvReleaseImage(&bigFrameCopy);
	cvDestroyAllWindows();
	
}

void GesKernel::captureVideoFrame()
{
	// Capture the next frame
	if (g_bigScreen) {
		cvResize(nextVideoFrame(), bigFrameCopy);
		cvResize(bigFrameCopy, smallFrameCopy);
	} else {
		cvResize(nextVideoFrame(), smallFrameCopy);
	}
	

	if( !bigFrameCopy ) 
		exitProgram(EXIT_FAILURE);		

	if( smallFrameCopy->origin == 1 ) { // 1 means the image is inverted

		cvFlip( smallFrameCopy, 0, 0 );
		smallFrameCopy->origin = 0;
	}

	
}





