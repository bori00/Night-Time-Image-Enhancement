#include "stdafx.h"
#include "BrightOnlyTransmissionEstimator.h"

Mat BrightOnlyTransmissionEstimator::getEstimation(const Mat& bright_channel_img, Vec3b atm_light) {
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
