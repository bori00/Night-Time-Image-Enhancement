#include "stdafx.h"
#include "BrightOnlyTransmissionEstimator.h"

std::vector<std::vector<double>> BrightOnlyTransmissionEstimator::getEstimation(const Mat& bright_channel_img, uchar atm_light) {
	int height = bright_channel_img.rows;
	int width = bright_channel_img.cols;

	std::vector<std::vector<double>> t(height);

	for (int i = 0; i < height; i++) {

		t[i].resize(width);

		for (int j = 0; j < width; j++) {

			double num = bright_channel_img.at<uchar>(i, j) - atm_light;
			double den = 255 - atm_light;

			t[i][j] = num / den;
		}
	}

	return t;
}
