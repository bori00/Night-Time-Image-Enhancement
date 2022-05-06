#pragma once

#include "stdafx.h"
#include "common.h"

class GuidedFilter
{
public:
	/*
	* p = input image
	* I = guidance image
	* r = window radius
	* epsilon = regularization strength
	*/
	Mat guide(Mat p, Mat I, int r, double epsilon);
};

