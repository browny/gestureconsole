
#include <stdio.h>
#include <string>
#include "cv.h"
#include "highgui.h"
#include "capture.h"
using namespace std;

CvCapture * pCapture = 0;

int initCapture(captureFrom src)
{
	if ( src == fromCam ) {
		pCapture = cvCreateCameraCapture(0);
	} 
	else if ( src == fromFile ) {
		string fileNameStr = "test.wmv";
		const char* fileName = fileNameStr.c_str();
		pCapture = cvCreateFileCapture(fileName);
	}

	if( !pCapture )
	{
		fprintf(stderr, "failed to initialize video capture\n");
		return 0;
	}

	return 1;
}

void closeCapture()
{
	// Terminate video capture and free capture resources
	cvReleaseCapture( &pCapture );
	return;
}

IplImage* nextVideoFrame()
{
	IplImage * pVideoFrame = cvQueryFrame( pCapture );
	if( !pVideoFrame )
		fprintf(stderr, "failed to get a video frame\n");

	return pVideoFrame;
}