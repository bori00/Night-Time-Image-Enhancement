#include "stdafx.h"
#include "PatchFilter.h"
#include <vector>

uchar ChannelIntensityFilter::getTopIntensity(const Vec3b& pixel, uchar curr_value) {
	for (int i = 0; i < 3; i++) {
		if (isHigher(pixel[i], curr_value)) {
			curr_value = pixel[i];
		}
	}
	return curr_value;
}

uchar ChannelIntensityFilter::getTopIntensity(const Vec3b& pixel) {
	return getTopIntensity(pixel, pixel[0]);
}

bool ChannelIntensityMaxFilter::isHigher(uchar a, uchar b) {
	return a > b;
}

bool ChannelIntensityMinFilter::isHigher(uchar a, uchar b) {
	return a < b;
}

Mat BruteForcePatchFilter::getFilteredImg(const Mat& img, int patch_radius, ChannelIntensityFilter* channel_internsity_filter) {
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

uchar BruteForcePatchFilter::getPatchCenterFilteredValue(const Mat& img, int patch_center_row, int patch_center_col, int patch_radius, ChannelIntensityFilter* channel_intensity_filter) {
	int height = img.rows;
	int width = img.cols;

	int min_row = max(patch_center_row - patch_radius, 0);
	int max_row = min(patch_center_row + patch_radius, height - 1);

	int min_col = max(patch_center_col - patch_radius, 0);
	int max_col = min(patch_center_col + patch_radius, width - 1);

	uchar curr_value = img.at<Vec3b>(patch_center_row, patch_center_col)[0];
	for (int i = min_row; i <= max_row; i++) {
		for (int j = min_col; j <= max_col; j++) {
			curr_value = channel_intensity_filter->getTopIntensity(img.at<Vec3b>(i, j), curr_value);
		}
	}

	return curr_value;
}

Mat DynamicPatchFilter::getFilteredImg(const Mat& img, int patch_radius, ChannelIntensityFilter* channel_internsity_filter) {
	int height = img.rows;
	int width = img.cols;

	Mat res = Mat(height, width, CV_8UC1);

	// ---- Pass 1: row-based aggregation

	// row_aggr_value[i][j] gives the filtered value for the column [j - patch_radius, j + patch_radius] in the row i
	std::vector<std::vector<uchar>> row_aggr_values(height);

	for (int i = 0; i < height; i++) {

		row_aggr_values[i].resize(width);
		
		std::deque<std::pair<int, int>> filtered_values_to_col;

		for (int j = 0; j < width + patch_radius; j++) {

			// drop old values from the beginning
			while (!filtered_values_to_col.empty() && filtered_values_to_col.front().second < j - 2 * patch_radius) {
				filtered_values_to_col.pop_front();
			}

			if (j < width) {
				uchar pixel_filtered_value = channel_internsity_filter->getTopIntensity(img.at<Vec3b>(i, j));

				// drop small values form the end
				while (!filtered_values_to_col.empty() && channel_internsity_filter->isHigher(pixel_filtered_value, filtered_values_to_col.back().first)) {
					filtered_values_to_col.pop_back();
				}

				filtered_values_to_col.push_back(std::make_pair(pixel_filtered_value, j));
			}

			if (j >= patch_radius) {
				row_aggr_values[i][j - patch_radius] = filtered_values_to_col.front().first;
			}
		}
	}

	// ---- pass 1: column-based aggregation

	for (int j = 0; j < width; j++) {

		std::deque<std::pair<int, int>> filtered_values_to_row;

		for (int i = 0; i < height + patch_radius; i++) {

			// drop old values from the beginning
			while (!filtered_values_to_row.empty() && filtered_values_to_row.front().second < i - 2 * patch_radius) {
				filtered_values_to_row.pop_front();
			}

			if (i < height) {

				uchar pixel_filtered_value = row_aggr_values[i][j];

				// drop small values form the end
				while (!filtered_values_to_row.empty() && channel_internsity_filter->isHigher(pixel_filtered_value, filtered_values_to_row.back().first)) {
					filtered_values_to_row.pop_back();
				}

				filtered_values_to_row.push_back(std::make_pair(pixel_filtered_value, i));
			}

			if (i >= patch_radius) {
				res.at<uchar>(i - patch_radius, j) = filtered_values_to_row.front().first;
			}
		}
	}

	return res;
}