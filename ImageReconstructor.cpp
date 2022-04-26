#include "stdafx.h"
#include "ImageReconstructor.h"

const double ImageReconstructor::t_threshold = 0.1;

Mat ImageReconstructor::reconstruct(const Mat& imgI, Vec3b atm_light, Mat& t) {
	int height = imgI.rows;
	int width = imgI.cols;

	Mat imgJ = Mat(height, width, CV_32FC3);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			for (int c = 0; c < 3; c++) {
				double curr_t = max(t_threshold, t.at<Vec3f>(i, j)[c]);
				imgJ.at<Vec3f>(i, j)[c] = ((double)imgI.at<Vec3b>(i, j)[c] - (double)atm_light[c]) / curr_t + atm_light[c];
				// printf("Computed value (%d, %d)[%d]: %lf, based on I=%d, t=%lf, A=%d\n", i, j, c, ((double)imgI.at<Vec3b>(i, j)[c] - (double)atm_light[c]) / curr_t + atm_light[c], imgI.at<Vec3b>(i, j)[c], curr_t, atm_light[c]);
			}
		}
	}

	Mat imgNormJ = Mat(height, width, CV_8UC3);

	cv::normalize(imgJ, imgNormJ, 0, 255, cv::NORM_MINMAX, CV_8U); //todo: normalization not mentioned in the article

	return imgNormJ;
}
