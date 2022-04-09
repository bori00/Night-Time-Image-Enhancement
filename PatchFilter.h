#pragma once

#include "stdafx.h"
#include "common.h"

class ChannelIntensityFilter
{
public:
	virtual uchar getFilteredIntensity(const Vec3b& pixel, uchar curr_value) = 0;
};

class ChannelIntensityMinFilter : public ChannelIntensityFilter
{
public:
	/*
	Given a 3-channel pixel (CV_8UC3) and a current_value, returns the
	min(curr_value, min(channel intensities of pixel)).
	*/
	uchar getFilteredIntensity(const Vec3b& pixel, uchar curr_value);
};

class ChannelIntensityMaxFilter : public ChannelIntensityFilter
{
public:
	/*
	Given a 3-channel pixel (CV_8UC3) and a current_value, returns the
	max(curr_value, max(channel intensities of pixel)).
	*/
	uchar getFilteredIntensity(const Vec3b& pixel, uchar curr_value);
};

class PatchFilter
{
public:
	static Mat getFilteredImg(const Mat& img, int patch_radius, ChannelIntensityFilter* channel_internsity_filter);

private:
	static uchar getPatchCenterFilteredValue(const Mat& img, int patch_center_row, int patch_center_col, int patch_radius, ChannelIntensityFilter* channel_intensity_filter);
};
