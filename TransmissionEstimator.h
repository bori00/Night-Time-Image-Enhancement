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
