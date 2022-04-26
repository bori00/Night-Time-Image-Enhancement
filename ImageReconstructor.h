#pragma once

#include "stdafx.h"
#include "common.h"
#include <vector>

class ImageReconstructor
{
public:
	static Mat reconstruct(const Mat& imgI, Vec3b atm_light, Mat& t);

private:
	static const double t_threshold;
};

