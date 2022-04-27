#include "stdafx.h"
#include "TransmissionEstimator.h"

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

Mat mat2gray(const Mat& src)
{
	Mat dst;
	normalize(src, dst, 0.0, 1.0, NORM_MINMAX);
	return dst;
}

Mat GuidedFilteringDoubleChannelTransmissionEstimator::getEstimation(const Mat& bright_channel_img, const Mat& dark_channel_img, Vec3b atm_light) {
	int height = dark_channel_img.rows;
	int width = dark_channel_img.cols;

	// CV_32FC3
	Mat guided_img = double_estimator_.getEstimation(bright_channel_img, dark_channel_img, atm_light);

	// guidance_img_: CV_32F

	imshow("Guidance Img", mat2gray(guidance_img_));

	Mat p; // mean of guided image
	blur(guided_img, p, Size(window_width_, window_width_));

	imshow("P", p);

	Mat mu; // mean of guidance image
	blur(guidance_img_, mu, Size(window_width_, window_width_));

	imshow("MU", mat2gray(mu));

	Mat sigma2; // variance of the guidance image
	// Note: variance = E[(img-E[img])^2] = E[img^2] - E[img]^2 =  guidance_img_square_mean = mu^2

	Mat guidance_img_square = guidance_img_.mul(guidance_img_);
	Mat guidance_img_square_mean;
	blur(guidance_img_square, guidance_img_square_mean, Size(window_width_, window_width_));

	imshow("Guidance Img Square Mean", mat2gray(guidance_img_square_mean));

	sigma2 = guidance_img_square_mean - mu.mul(mu);
	Mat sigma;
	sqrt(sigma2, sigma);

	imshow("Sigma", mat2gray(sigma));

	Mat IiniPi = guidance_img_.mul(p);

	Mat IiniPi_mean;
	blur(IiniPi, IiniPi_mean, Size(window_width_, window_width_));

	Mat MukPk = mu.mul(p);

	// equation 15
	Mat a = (IiniPi_mean - MukPk) / (sigma2 + epsylon_);

	// equation 16
	Mat b = p - a.mul(mu);

	// equation 17
	Mat t_guided(height, width, CV_32FC3);
	t_guided = a.mul(guided_img) + b;

	return t_guided;
}