#include "stdafx.h"
#include "TransmissionEstimator.h"

Mat BrightOnlyTransmissionEstimator::getEstimation(const Mat& bright_channel_img, const Mat& dark_channel_img, Vec3b atm_light) {
	int height = bright_channel_img.rows;
	int width = bright_channel_img.cols;

	Mat t(height, width, CV_32FC3);

	/*float min = 100;
	float max = -100;*/

	for (int i = 0; i < height; i++) {

		for (int j = 0; j < width; j++) {

			for (int c = 0; c < 3; c++) {

				float num = bright_channel_img.at<uchar>(i, j) - atm_light[c];
				float den = 255 - atm_light[c];

				/*if (num < 0) {
					printf("Here");
				}*/

				t.at<Vec3f>(i, j)[c] = (float)num / den;

				/*if ((float)num / den < min) {
					min = (float) num / den;
				}
				if ((float)num / den > max) {
					max = (float)num / den;
				}*/
			}
		}
	}

	// normalize t
	// t = (t - min) / (max - min);

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