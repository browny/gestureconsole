
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>

#include <cv.h>

#include "lookup_table.h"

#define _LOOKUP_TABLE

/*
 * 	Skin Color Detection:
 *		Using the explicitly defined skin region.
 *	reference from: 
 *		M. A. Berbar, H. M. Kelash and A. A. Kandeel, 
 *		¡§Faces and Facial Features Detection in Color Images,¡¨ 
 *		Proceedings of the IEEE International Conference on Geometric Modeling and Imaging--New Trends, pp. 209-214.
 *
 *	@brief	Skin color detection
 *	@param	image	input image buffer with RGB color space
 *	@ret	the result of skin color detection 
 */

IplImage* cvSkinDetect_Definitely(IplImage *image)
{
    int i;
	int width = image->width;
	int height = image->height;
    int imagesize = width * height;
    static IplImage *curr = 0;
    static IplImage *frame = 0;
	unsigned char Y, U, V, UV;
	unsigned char *p_out;
    unsigned char *p_data;

    if (!curr)
        curr = cvCreateImage( cvSize(width, height), IPL_DEPTH_8U, image->nChannels);
    if (!frame)
        frame = cvCreateImage( cvSize(width, height), IPL_DEPTH_8U, image->nChannels);
    cvCvtColor(image, curr, CV_BGR2YCrCb);

    p_data = (unsigned char*) curr->imageData;
    p_out = (unsigned char*) frame->imageData;

#ifdef _TIME
	double t = (double)cvGetTickCount();
#endif

    for (i = 0; i < imagesize; ++i) {
        /* Get y, u, v components */
        Y = p_data[0];
        U = p_data[1];
        V = p_data[2];

        p_out[0] = p_out[1] = p_out[2] = 0;

        /* Start to skin color detection */
        if (V > 137 && V < 177) {
            if (U > 77 && U < 127) {
                UV = U + 0.6 * V;
                if (UV > 175 && UV < 215) {
                    p_out[0] = 255;
                    p_out[1] = 255;
                    p_out[2] = 255;
                }
            }
        }
        ++p_out; ++p_out; ++p_out;
        ++p_data; ++p_data; ++p_data;
    }

#ifdef _TIME
	t = (double)cvGetTickCount() - t;
	syfCv_log(LOG_INFO, 0, "%gms\n", t/((double)cvGetTickFrequency()*1000.) );
#endif
	
	return frame;
}

/*
 * 	Skin Color Detection:
 *		Using the skin color ellipse model.
 *	reference form:
 *		J. Kovac, P. Peer and F. Solina, 
 *		"Human Skin Color Clustering for Face Detection,¡¨ 
 *		Proceedings of the IEEE Region 8 Computer as a Tool, 2003, Vol. 2, pp. 144-148
 *
 *	@brief	Skin color detection
 *	@param	image	input image buffer with YCbCr color space
 *	@ret	the result of skin color detection 
 */
void cvSkinDetect_Ellipse(const IplImage *image, IplImage* out, IplImage* curr)
{
    int i;
	int width = image->width;
	int height = image->height;
    int imagesize = width * height;
    //IplImage *curr = 0;
    //static IplImage *out = 0;

    /*if (!curr)
        curr = cvCreateImage( cvSize(width, height), IPL_DEPTH_8U, image->nChannels);*/
    /*if (!out)
        out = cvCreateImage( cvSize(width, height), IPL_DEPTH_8U, 1);*/
    cvCvtColor(image, curr, CV_BGR2YCrCb);

    unsigned char *p_data = (unsigned char*) curr->imageData;
    unsigned char *p_out = (unsigned char*) out->imageData;
	unsigned char Cr, Cb;

#ifdef _LOOKUP_TABLE
	int result;
	int nx, ny;
#else
	double result;
#endif

    for (i = 0; i < imagesize; ++i) {

		//std::cout << i << std::endl;

        ++p_data;
        Cr = *p_data++;
        Cb = *p_data++;

        /*
         *  x = cos_theta*(Cb_p - Cx) + sin_theta*(Cr_p - Cy)
         *  y = cos_theta*(Cr_p - Cy) - sin_theta*(Cb_p - Cx)
         */
#ifndef _LOOKUP_TABLE
		double Cb_p, Cr_p;
		double x, y;
		Cb_p = Cb;
		Cr_p = Cr;
        x = cos(2.53)*(Cb_p - Cx) + sin(2.53)*(Cr_p - Cy);
        y = cos(2.53)*(Cr_p - Cy) - sin(2.53)*(Cb_p - Cx);

        result = ((x - eCx)*(x - eCx)) / (a*a) + ((y - eCy)*(y - eCy)) / (b*b);
#else
        /*
         * @ Do not apply x, y calculation.
         * result = ((x - eCx)*(x - eCx)/exp_a) + ((y - eCy)*(y - eCy)/exp_b);
         */
		nx = NUMERATOR_X_RESULT[p_data[2]][p_data[1]];
		ny = NUMERATOR_Y_RESULT[p_data[1]][p_data[2]];
		result = (nx == -1 || ny == -1) ? 1004 : nx + ny;
#endif

#ifdef _LOOKUP_TABLE
        if (result <= 1000)	// fixed-point
#else
		if (result <= 1)
#endif
		{
            *p_out = 255;
        }
        else {
        	*p_out = 0;
        }
        ++p_out;
    }
		
}
