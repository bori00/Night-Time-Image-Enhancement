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

	Vec3b getEstimate(const Mat& img, const Mat& bright_channel_img);

private:
	double ratio_of_bright_pixels_;

	std::vector<std::pair<int, int>> getTopBrightestPixelCoords(const Mat& bright_channel_img);

	Vec3b getMeanOfSelectedPixels(const Mat& img, std::vector<std::pair<int, int>> selected_coords);
};

