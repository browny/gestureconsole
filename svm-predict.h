#ifndef __SVM_PREDICT_H__
#define __SVM_PREDICT_H__

#include <stdio.h>
//#include <ctype.h>
#include <stdlib.h>
#include <string.h>
//#include <errno.h>
#include "svm.h"

void predict(double *input_feature, int len, double *prob_estimates);
void svmUnload();

#endif

