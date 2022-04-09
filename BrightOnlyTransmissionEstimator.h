#pragma once

#include "stdafx.h"
#include "common.h"
#include<vector>

class BrightOnlyTransmissionEstimator
{
public:
	
	static Mat getEstimation(const Mat& bright_channel_img, Vec3b atm_light);
};

