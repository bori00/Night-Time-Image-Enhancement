#pragma once

#include "stdafx.h"
#include "common.h"
#include<vector>

class TransmissionEstimator
{
public:
	virtual Mat getEstimation(const Mat& bright_channel_img, const Mat& dark_channel_img, Vec3b atm_light) = 0;
};

class BrightOnlyTransmissionEstimator : public TransmissionEstimator
{
public:
	 Mat getEstimation(const Mat& bright_channel_img, const Mat& dark_channel_img, Vec3b atm_light) override;
};

class DarkOnlyTransmissionEstimator : public TransmissionEstimator
{
public:
	Mat getEstimation(const Mat& bright_channel_img, const Mat& dark_channel_img, Vec3b atm_light) override;
};

class DoubleChannelTransmissionEstimator : public TransmissionEstimator
{
public:
	DoubleChannelTransmissionEstimator(double alpha) : alpha_(alpha) {}

	Mat getEstimation(const Mat& bright_channel_img, const Mat& dark_channel_img, Vec3b atm_light) override;

private:
	BrightOnlyTransmissionEstimator bright_estimator_;
	DarkOnlyTransmissionEstimator dark_estimator_;
	double alpha_;
};


class GuidedFilteringDoubleChannelTransmissionEstimator : public TransmissionEstimator
{
public:
	GuidedFilteringDoubleChannelTransmissionEstimator(double alpha, double epsylon, int window_width, Mat guidance_img) : 
		alpha_(alpha), 
		epsylon_(epsylon),
		window_width_(window_width),
		double_estimator_(alpha)
	{
		guidance_img.convertTo(guidance_img_, CV_32FC3);
		cv::normalize(guidance_img_, guidance_img_, 0, 1, cv::NORM_MINMAX);
		imshow("1. Guidance Img", guidance_img_);
	}

	Mat getEstimation(const Mat& bright_channel_img, const Mat& dark_channel_img, Vec3b atm_light) override;

private:
	DoubleChannelTransmissionEstimator double_estimator_;
	double alpha_;
	double epsylon_;
	int window_width_;
	Mat guidance_img_;
};