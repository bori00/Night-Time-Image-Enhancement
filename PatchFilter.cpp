#include "stdafx.h"
#include "PatchFilter.h"

uchar ChannelIntensityMinFilter::getFilteredIntensity(const Vec3b& pixel, uchar curr_value) {
	for (int i = 0; i < 3; i++) {
		if (curr_value > pixel[i]) {
			curr_value = pixel[i];
		}
	}
	return curr_value;
}

uchar ChannelIntensityMaxFilter::getFilteredIntensity(const Vec3b& pixel, uchar curr_value) {
	for (int i = 0; i < 3; i++) {
		if (curr_value < pixel[i]) {
			curr_value = pixel[i];
		}
	}
	return curr_value;
}


Mat PatchFilter::getFilteredImg(const Mat& img, int patch_radius, ChannelIntensityFilter* channel_internsity_filter) {
	int height = img.rows;
	int width = img.cols;

	Mat res = Mat(height, width, CV_8UC1);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			// patch center: (i, j)
			res.at<uchar>(i, j) = getPatchCenterFilteredValue(img, i, j, patch_radius, channel_internsity_filter);
		}
	}

	return res;
}

uchar PatchFilter::getPatchCenterFilteredValue(const Mat& img, int patch_center_row, int patch_center_col, int patch_radius, ChannelIntensityFilter* channel_intensity_filter) {
	int height = img.rows;
	int width = img.cols;

	int min_row = max(patch_center_row - patch_radius, 0);
	int max_row = min(patch_center_row + patch_radius, height - 1);

	int min_col = max(patch_center_col - patch_radius, 0);
	int max_col = min(patch_center_col + patch_radius, width - 1);

	uchar curr_value = img.at<Vec3b>(patch_center_row, patch_center_col)[0];
	for (int i = min_row; i <= max_row; i++) {
		for (int j = min_col; j <= max_col; j++) {
			curr_value = channel_intensity_filter->getFilteredIntensity(img.at<Vec3b>(i, j), curr_value);
		}
	}

	return curr_value;
}