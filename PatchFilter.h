#pragma once

#include "stdafx.h"
#include "common.h"

class ChannelIntensityFilter
{
public:
	uchar getTopIntensity(const Vec3b& pixel, uchar curr_value);

	uchar getTopIntensity(const Vec3b& pixel);

	virtual bool isHigher(uchar a, uchar b) = 0;
};

class ChannelIntensityMinFilter : public ChannelIntensityFilter
{
public:
	bool isHigher(uchar a, uchar b);
};

class ChannelIntensityMaxFilter : public ChannelIntensityFilter
{
public:
	bool isHigher(uchar a, uchar b);
};

class PatchFilter
{
public:
	virtual Mat getFilteredImg(const Mat& img, int patch_radius, ChannelIntensityFilter* channel_internsity_filter) = 0;

private:
	static uchar getPatchCenterFilteredValue(const Mat& img, int patch_center_row, int patch_center_col, int patch_radius, ChannelIntensityFilter* channel_intensity_filter);
};

class BruteForcePatchFilter : public PatchFilter
{
public:
	Mat getFilteredImg(const Mat& img, int patch_radius, ChannelIntensityFilter* channel_internsity_filter);

private:
	static uchar getPatchCenterFilteredValue(const Mat& img, int patch_center_row, int patch_center_col, int patch_radius, ChannelIntensityFilter* channel_intensity_filter);
};

class DynamicPatchFilter : public PatchFilter
{
public:
	Mat getFilteredImg(const Mat& img, int patch_radius, ChannelIntensityFilter* channel_internsity_filter);
};
