
#ifndef __CAPTURE_H__
#define __CAPTURE_H__

enum captureFrom
{
	fromCam = 0,
	fromFile
};

int  initCapture(captureFrom src);
void closeCapture();
IplImage * nextVideoFrame();

#endif

