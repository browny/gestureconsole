#ifndef __SKINALGO_H__
#define __SKINALGO_H__

#include <cv.h>

void cvSkinDetect_Ellipse(const IplImage *image, IplImage* out, IplImage* curr);
IplImage* cvSkinDetect_Definitely(IplImage *image);

#endif	/// __ALGORITHM_H__
