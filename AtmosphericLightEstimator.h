#pragma once

#include "stdafx.h"
#include "common.h"
#include <map>

class AtmosphericLightEstimator
{
public:
	/*
	* ratio_of_bright_pixels = the ratio of top brightest pixels whose means is considered
	* to approximate the global atmospheric light. Range: (0, 1]
	*/
	AtmosphericLightEstimator(double ratio_of_bright_pixels);

	uchar getEstimate(const Mat& img);

private:
	double ratio_of_bright_pixels_;

	void removeLowestTopBrightValue(std::map<uchar, int>& values_to_freq);

	void addBrightValueToTop(std::map<uchar, int>& values_to_freq, uchar value);

	uchar getWeightedMean(const std::map<uchar, int>& values_to_freq);
};

