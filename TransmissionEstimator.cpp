#include "stdafx.h"
#include "TransmissionEstimator.h"
#include "GuidedFilter.h"

Mat BrightOnlyTransmissionEstimator::getEstimation(const Mat& bright_channel_img, const Mat& dark_channel_img, Vec3b atm_light) {
	int height = bright_channel_img.rows;
	int width = bright_channel_img.cols;

	Mat t(height, width, CV_32FC3);

	for (int i = 0; i < height; i++) {

		for (int j = 0; j < width; j++) {

			for (int c = 0; c < 3; c++) {

				float num = bright_channel_img.at<uchar>(i, j) - atm_light[c];
				float den = 255 - atm_light[c];

				t.at<Vec3f>(i, j)[c] = (float)num / den;
			}
		}
	}

	return t;
}

Mat DarkOnlyTransmissionEstimator::getEstimation(const Mat& bright_channel_img, const Mat& dark_channel_img, Vec3b atm_light) {
	int height = dark_channel_img.rows;
	int width = dark_channel_img.cols;

	Mat t(height, width, CV_32FC3);

	for (int i = 0; i < height; i++) {

		for (int j = 0; j < width; j++) {

			for (int c = 0; c < 3; c++) {

				t.at<Vec3f>(i, j)[c] = 1 - ((float)dark_channel_img.at<uchar>(i, j)) / ((float)atm_light[c]);
			}
		}
	}

	return t;
}

Mat DoubleChannelTransmissionEstimator::getEstimation(const Mat& bright_channel_img, const Mat& dark_channel_img, Vec3b atm_light) {
	int height = dark_channel_img.rows;
	int width = dark_channel_img.cols;

	Mat t_corrected(height, width, CV_32FC3);

	Mat t_bright = bright_estimator_.getEstimation(bright_channel_img, dark_channel_img, atm_light);

	Mat t_dark = dark_estimator_.getEstimation(bright_channel_img, dark_channel_img, atm_light);

	for (int i = 0; i < height; i++) {

		for (int j = 0; j < width; j++) {

			if (bright_channel_img.at<uchar>(i, j) - dark_channel_img.at<uchar>(i, j) < alpha_ * UCHAR_MAX) {
				// do not trust bright-only estimation, correction required
				for (int c = 0; c < 3; c++) {
					t_corrected.at<Vec3f>(i, j)[c] = t_bright.at<Vec3f>(i, j)[c] * t_dark.at<Vec3f>(i, j)[c];
				}
			}
			else {
				// no correction applied, use t_bright only
				t_corrected.at<Vec3f>(i, j) = t_bright.at<Vec3f>(i, j);
			}

			
		}
	}

	return t_corrected;
}

Mat GuidedFilteringDoubleChannelTransmissionEstimator::getEstimation(const Mat& bright_channel_img, const Mat& dark_channel_img, Vec3b atm_light) {
	int height = dark_channel_img.rows;
	int width = dark_channel_img.cols;

	Mat double_channel_t = double_estimator_.getEstimation(bright_channel_img, dark_channel_img, atm_light);

	cv::cvtColor(double_channel_t, double_channel_t, COLOR_BGR2GRAY);
	cv::cvtColor(img_, img_, COLOR_BGR2GRAY);

	GuidedFilter guided_filter;
	Mat refined_t = guided_filter.guide(double_channel_t, img_, window_width_ / 2, epsylon_);

	cv::cvtColor(refined_t, refined_t, COLOR_GRAY2BGR);

	return refined_t;
}