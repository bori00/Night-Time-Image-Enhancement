#pragma once

#include "stdafx.h"
#include "common.h"
#include<vector>

class BrightOnlyTransmissionEstimator
{
public:
	static std::vector<std::vector<double>> getEstimation(const Mat& bright_channel_img, uchar atm_light);
};

