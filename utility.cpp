
#include "utility.h"

CvScalar ColorBox[4] = {CV_RGB(255,0,0), CV_RGB(0,255,0), CV_RGB(0,0,255), CV_RGB(255,0,255)};

void connectComponent(IplImage* src, const int poly_hull0, const float perimScale, 
					  int* num, RectArr &rects, vector<CvPoint> &centers)
{
	/*
	* Pre : "src" is the input image 
	*       "poly_hull0" is usually set to 1
	*       "perimScale" defines how big connected component will be retained, bigger 
	*                    the number, more components are retained (100)
	* Post: "num" defines how many connected component was found
	*       "rects" the bounding box of each connected component
	*       "centers" the center of each bounding box
	*/     

	CvMemStorage* mem_storage = NULL;
	CvSeq* contours = NULL;		
	

	// Clean up	
	cvMorphologyEx(src, src, 0, 0, CV_MOP_OPEN, 1);
	cvMorphologyEx(src, src, 0, 0, CV_MOP_CLOSE, 1);
	

	// Find contours around only bigger regions
	mem_storage = cvCreateMemStorage(0);
	
	CvContourScanner scanner = cvStartFindContours(src, mem_storage, sizeof(CvContour), 
		                                           CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	CvSeq* c;
	int numCont = 0;
		
	while ((c = cvFindNextContour(scanner)) != NULL) {

		double len = cvContourPerimeter(c);

		// calculate perimeter len threshold
		double q = (double)(src->height + src->width)/perimScale;

		// get rid of blob if its perimeter is too small
		if (len < q) {
			cvSubstituteContour(scanner, NULL);
		} else {

			// smooth its edge if its large enough						
			CvSeq* c_new;
			if (poly_hull0) {

				// polygonal approximation
				c_new = cvApproxPoly(c, sizeof(CvContour), mem_storage, CV_POLY_APPROX_DP, 2, 0);

			} else {

				// convex hull of the segmentation
				c_new = cvConvexHull2(c, mem_storage, CV_CLOCKWISE, 1);

			}

			cvSubstituteContour(scanner, c_new);
						
			numCont++;
		}
	}		

	contours = cvEndFindContours(&scanner);


	// Calc center of mass and/or bounding rectangles
	if (num != NULL) {

		// user wants to collect statistics
		int numFilled = 0, i = 0;

		for (i = 0, c = contours; c != NULL; c = c->h_next, i++) {

			if (i < *num) {
				
				// bounding retangles around blobs				
				
				rects.push_back( cvBoundingRect(c) );			

				CvPoint center = cvPoint(rects[i].x + rects[i].width  / 2,
					rects[i].y + rects[i].height / 2);
				centers.push_back(center);

				numFilled++;
			}
		}

		*num = numFilled;		

	}	

	cvReleaseMemStorage(&mem_storage);

}
void drawCC(IplImage* targetFrame, const int num, const RectArr &rects, const PointArr &centers)
{
	/*
	*  Draw the connected component centers and bounding boxes to "targetFramw"
	*/

	if (num != 0) {
		for (int i = 0; i < num; ++i) {
			cvCircle(targetFrame, centers[i], 5, CV_RGB(0x00, 0xff, 0xff), -1);
			cvRectangle(targetFrame, cvPoint(rects[i].x, rects[i].y),
				cvPoint(rects[i].x + rects[i].width,  rects[i].y + rects[i].height), 
				CV_RGB(255, 255, 0), 1);
		}
	}

}
void resampleByPoints(const IplImage* input, const int srMar, const PointArr &points, 
					  IplImage* output)
{	

	/*
	*  Pre:  "input" is the source to be resampled
	*        "srMar" is the distant from resampling center to form a square
	*        "points" I take these points as centers to select square resampling area
	*
	*  Post: "output" the resampling result
	*/
	
	cvSetZero(output); // clear output image
	CvSize sz = cvGetSize(output); 
	
	for (unsigned int i = 0; i < points.size(); ++i) {

		CvPoint leftTop     = cvPoint(points[i].x - srMar, points[i].y - srMar);
		CvPoint rightBottom = cvPoint(points[i].x + srMar, points[i].y + srMar);

		if (leftTop.x < 0)
			leftTop.x = 0;

		if (leftTop.y < 0)
			leftTop.y = 0;

		if (rightBottom.x > sz.width)
			rightBottom.x = sz.width;

		if (rightBottom.y > sz.height)
			rightBottom.y = sz.height;

		for (int j = leftTop.y; j < rightBottom.y; j++) {
			
			uchar* ptr = (uchar*)(input->imageData + j*input->widthStep);
			uchar* ptrNew = (uchar*)(output->imageData + j*output->widthStep);

			for (int k = leftTop.x; k < rightBottom.x; k++)
				ptrNew[k] = ptr[k];									
		}
	}
}
void kMeans(const PointArr &dataVector, const int clusterCount, PointIn2DArr &clusterContainer)
{
	/*
	*  Pre:  "dataVector" the data to be clustered by K-Means 
	*        "clusterCount" how many clusters you want
	*
	*  Post: "classContainer" I pack the points with the same cluster into vector, so it 
	*        is a vetor of vector
	*/

	int dataLength = dataVector.size();
		
	// Put data into suitable container
	CvMat* points   = cvCreateMat(dataLength, 1, CV_32FC2);
	CvMat* clusters = cvCreateMat(dataLength, 1, CV_32SC1 );

	for (int row = 0; row < points->rows; row++) {		
		float* ptr = (float*)(points->data.ptr + row*points->step);
		for (int col = 0; col < points->cols; col++) {
			*ptr = static_cast<float>(dataVector[row].x);
			ptr++;
			*ptr = static_cast<float>(dataVector[row].y);
		}
	}

	// The Kmeans algorithm function (OpenCV function)
	cvKMeans2(points, clusterCount, clusters, cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 1, 2));

	// Pack result to 'classContainer': each element in 'classContainer' means one cluster,
	// each cluster is one PointArr contain all points belong to this cluster
	packClusters(clusterCount, points, clusters, clusterContainer);
	
	removeEmptyClusters(clusterContainer);
		
	cvReleaseMat(&points);
	cvReleaseMat(&clusters);

}
void packClusters(int clusterCount, const CvMat* points, CvMat* clusters, PointIn2DArr &clusterContainer)
{
	for (int i = 0; i < clusterCount; i++) {		

		PointArr tempClass;	

		for (int row = 0; row < clusters->rows; row++) {

			float* p_point = (float*)(points->data.ptr + row*points->step);
			int X = static_cast<int>(*p_point) ;
			p_point++;
			int Y = static_cast<int>(*p_point);			

			if (clusters->data.i[row] == i)
				tempClass.push_back(cvPoint(X, Y));
		}

		clusterContainer.push_back(tempClass);						

	} 
}
void removeEmptyClusters(PointIn2DArr &clusterContainer)
{
	for (unsigned int i = 0; i < clusterContainer.size(); ++i) {		

		if ( clusterContainer[i].empty() ) {			
			PointIn2DArr::iterator iter = clusterContainer.begin();
			iter = iter + i;
			clusterContainer.erase(iter);	
			i = i - 1;
		}
	}	
}
void getClusterCenters(PointIn2DArr &points2DArr, PointArr &centers)
{
	/*
	*  Pre : "clusterContainer" is classfied point set by K-Means
	*
	*  Post: "clusterMass" is the calculated mass center of each classified cluster points
	*/

	for (unsigned int i = 0; i < points2DArr.size(); i++) {

		int x = 0, y = 0;
		PointArr tempClass(points2DArr[i]);

		for (unsigned int j = 0; j < tempClass.size(); j++) {
			x += tempClass[j].x;
			y += tempClass[j].y;							
		}

		centers.push_back(cvPoint(x/tempClass.size(), y/tempClass.size()));

	}
}
int mergeClusters(PointArr &clusterCenters, PointIn2DArr &clusterPoints, double MERGE_TH)
{	
	/*
	*  Pre:  "clusterMass" mass center of "clusterContainer"
	*        "clusterContainer" is the container of the clustered points
	*        "MERGE_TH" defines how near two "clusterMass" will be merged
    *
	*  Post: return the remaining cluster count after merging
	*/
	
	bool mergeDone = false;

	while (mergeDone != true) {

		CvPoint toFrom = findMostSimilar(clusterCenters, MERGE_TH, &mergeDone);

		if (mergeDone == false) 
			merging(toFrom, clusterCenters, clusterPoints);

	}

	return clusterCenters.size();
	
}

CvPoint findMostSimilar(PointArr &clusterCenters, double MERGE_TH, bool* mergeDone)
{

	*mergeDone = true;
	
	int mergeToIndex = 0;
	int mergeFromIndex = 0;	
	double centerDis = 0;
	double minDis = 1000000;
	
	for (unsigned int i = 0; i < clusterCenters.size()-1; ++i) {
		for (unsigned int j = i+1; j < clusterCenters.size(); ++j) {							

			centerDis = getPointDistant(clusterCenters[i], clusterCenters[j]);

			if ((centerDis < MERGE_TH) && (centerDis < minDis)) {

				minDis = centerDis;

				if (i < j) {
					mergeToIndex = i;
					mergeFromIndex = j;
				} else {
					mergeToIndex = j;
					mergeFromIndex = i;
				}

				*mergeDone = false;

			}
		}									
	}

	return cvPoint(mergeToIndex, mergeFromIndex);

}


void merging(CvPoint toFrom, PointArr &clusterCenters, PointIn2DArr &clusterPoints)
{
	PointIn2DArr mergedClusterPoints;
	PointArr mergedClusterCenters;	
	unsigned int toIdx = (unsigned int)toFrom.x;
	unsigned int frIdx = (unsigned int)toFrom.y;

	for (unsigned int i = 0; i < clusterCenters.size(); ++i) {

		if (i == toIdx) {

			PointArr tempClass(clusterPoints[toIdx]);

			tempClass.insert(tempClass.end(), clusterPoints[frIdx].begin(), clusterPoints[frIdx].end());

			mergedClusterPoints.push_back(tempClass);

		} else if ((i != toIdx) && (i != frIdx)) {

			mergedClusterPoints.push_back(clusterPoints[i]);

		}

	}

	getClusterCenters(mergedClusterPoints, mergedClusterCenters);

	clusterPoints.clear();
	clusterCenters.clear();

	clusterPoints = mergedClusterPoints;
	clusterCenters = mergedClusterCenters;
	
}

int clustering(const PointArr &points, int clusterNum, double mergeRange, 
			   PointIn2DArr &clusterContainer, PointArr &clusterMass)
{
	/*
	*  Pre:  "points" are the data points you want to clustering by K-Means
	*        "clusterNum" defines how many cluster you want
	*        "mergeRange" defines the distance threshold between different clusters
	*
	*  Post: "clusterContainer" is vector of clustered points vector
	*        "clusterMass" is vector of the mass center of clustered points vector
	*/	

	kMeans(points, clusterNum, clusterContainer);	
	getClusterCenters(clusterContainer, clusterMass);		
	
	return mergeClusters(clusterMass, clusterContainer, mergeRange);

}


void refineSkinArea(const IplImage* skin, 
					int clusterNum, PointArr &points, 
					IplImage* output, int* num, RectArr &rects, PointArr &centers)
{
	// Use clustring to refine the detected skin area
	if (points.size() != 0) {

		PointArr clusterMass;
		PointIn2DArr clusterContainer;				
		
		clustering(points, clusterNum, 50, clusterContainer, clusterMass);
		resampleByPoints(skin, 20, clusterMass, output);		
		//cvDilate(output, output);		
		connectComponent(output, 1, 200, num, rects, centers);
		

	} else {

		*num = 0;
		rects.clear();
		centers.clear();

	}
	
			
}

CvPoint avgCvPoints(const PointArr &points, int num)
{
	// Calculate the average of the points vector excluding (-1, -1)

	int bufRealCenterCount = 0;
	int bufAccCenter_x = 0, bufAccCenter_y = 0;
	
	for (int i = 0; i < num; i++) {
		if ( (points[i].x != -1) && (points[i].y != -1) ) {
			bufAccCenter_x += points[i].x;
			bufAccCenter_y += points[i].y;
			bufRealCenterCount++;
		}
	}

	if (bufRealCenterCount > 0) {
		int bufCenter_x = bufAccCenter_x/bufRealCenterCount;
		int bufCenter_y = bufAccCenter_y/bufRealCenterCount;
		
		return cvPoint(bufCenter_x, bufCenter_y); 			

	} else {
		
		// if all points in buffer are (-1, -1), the hand point will disappear
		return cvPoint(-1, -1);
	}	

}
void drawHands(IplImage* targetFrame, const PointArr &currentHand, int num, CvSize sz)
{	

	//CvScalar ColorBox[4] = {CV_RGB(255,0,0), CV_RGB(0,255,0), CV_RGB(0,0,255), CV_RGB(255,0,255)};
	const int REC_SIZE = 15;

	for (int i = 0; i < num; ++i) {
		if (currentHand[i].x != -1) {

			cvCircle(targetFrame, currentHand[i], 5, ColorBox[i], -2);

			int x1 = 0;
			int y1 = 0;
			int x2 = 0;
			int y2 = 0;

			if ( (currentHand[i].x - REC_SIZE) < 0 )
				x1 = 0;
			else
				x1 = currentHand[i].x - REC_SIZE;

			if ( (currentHand[i].y - REC_SIZE) < 0 )
				y1 = 0;
			else
				y1 = currentHand[i].y - REC_SIZE;

			if ( (currentHand[i].x + REC_SIZE) >= sz.width )
				x2 = sz.width - 1;
			else
				x2 = currentHand[i].x + REC_SIZE;

			if ( (currentHand[i].y + REC_SIZE) >= sz.height )
				y2 = sz.height - 1;
			else
				y2 = currentHand[i].y + REC_SIZE;

			CvPoint leftTop  = cvPoint(x1, y1);
			CvPoint rightBot = cvPoint(x2, y2);
			cvRectangle(targetFrame, leftTop, rightBot, ColorBox[i], 2);
			//cout << "x: " << currentHand[i].x << " y: " << currentHand[i].y << endl;
		} else {

			/*ColorBox[0] = ColorBox[3];
			ColorBox[3] = ColorBox[2];
			ColorBox[2] = ColorBox[1];
			ColorBox[1] = ColorBox[0];*/

		}
		
	}	

}


void drawHandsBig(IplImage* targetFrame, const PointArr &currentHand, int num, CvSize sz)
{	

	PointArr currentHandBig(currentHand.size());
	for (unsigned int i = 0; i < currentHand.size(); ++i) {
		CvPoint temp = currentHand[i];
		currentHandBig[i] = cvPoint(2*temp.x, 2*temp.y);
	}

	const int REC_SIZE = 30;

	for (int i = 0; i < num; ++i) {
		if (currentHandBig[i].x != -2) {

			cvCircle(targetFrame, currentHandBig[i], 5, ColorBox[i], -2);

			int x1 = 0;
			int y1 = 0;
			int x2 = 0;
			int y2 = 0;

			if ( (currentHandBig[i].x - REC_SIZE) < 0 )
				x1 = 0;
			else
				x1 = currentHandBig[i].x - REC_SIZE;

			if ( (currentHandBig[i].y - REC_SIZE) < 0 )
				y1 = 0;
			else
				y1 = currentHandBig[i].y - REC_SIZE;

			if ( (currentHandBig[i].x + REC_SIZE) >= sz.width )
				x2 = sz.width - 1;
			else
				x2 = currentHandBig[i].x + REC_SIZE;

			if ( (currentHandBig[i].y + REC_SIZE) >= sz.height )
				y2 = sz.height - 1;
			else
				y2 = currentHandBig[i].y + REC_SIZE;

			CvPoint leftTop  = cvPoint(x1, y1);
			CvPoint rightBot = cvPoint(x2, y2);
			cvRectangle(targetFrame, leftTop, rightBot, ColorBox[i], 2);
			//cout << "x: " << currentHand[i].x << " y: " << currentHand[i].y << endl;

		} else {

			/*ColorBox[0] = ColorBox[3];
			ColorBox[3] = ColorBox[2];
			ColorBox[2] = ColorBox[1];
			ColorBox[1] = ColorBox[0];*/

		}
		
	}	

}

void showMaskPart(const IplImage* src, const IplImage* mask, IplImage* result)
{
    /* src is the source image which you want to mask
     * mask is a single channel binary image as a mask
     * result is the image with the same size, depth, channel with src
     */
 
    cvZero(result);
 
    CvSize sz = cvSize(src->width, src->height);	
    IplImage* refImg = cvCreateImage(sz, src->depth, src->nChannels);
    cvZero(refImg);
 
    cvOr(src, refImg, result, mask);
 
    cvReleaseImage(&refImg);
 
}

void maskByRects(const IplImage* src, const RectArr &rects, IplImage* result)
{
	cvZero(result);

	for (unsigned int i = 0; i < rects.size(); ++i) {

		CvPoint leftTop     = cvPoint(rects[i].x, rects[i].y);
		CvPoint rightBottom = cvPoint(rects[i].x + rects[i].width, rects[i].y + rects[i].height);

		for (int j = leftTop.y; j < rightBottom.y; j++) {

			uchar* ptr = (uchar*)(src->imageData + j*src->widthStep);
			uchar* ptrNew = (uchar*)(result->imageData + j*result->widthStep);

			for (int k = leftTop.x; k < rightBottom.x; k++)
				ptrNew[k] = ptr[k];									
		}
	}
}
int getMaxRect(RectArr &rects)
{
	RectArr::iterator it = rects.begin();

	int maxArea = 0;

	while (it != rects.end()) {

		if ( (it->width * it->height) > maxArea)
			maxArea = it->width * it->height;

		it++;								
	}

	maxArea = (maxArea / 100) * 100;

	return maxArea;

}
