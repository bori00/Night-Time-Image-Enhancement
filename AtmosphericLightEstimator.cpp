#include "stdafx.h"
#include "AtmosphericLightEstimator.h"

AtmosphericLightEstimator::AtmosphericLightEstimator(double ratio_of_bright_pixels)
	: ratio_of_bright_pixels_(ratio_of_bright_pixels) {}

Vec3b AtmosphericLightEstimator::getEstimate(const Mat& img, const Mat& bright_channel_img) {
	std::vector<std::pair<int, int>> brightest_pixel_coords = getTopBrightestPixelCoords(bright_channel_img);

	return getMeanOfSelectedPixels(img, brightest_pixel_coords);
}

std::vector<std::pair<int, int>> AtmosphericLightEstimator::getTopBrightestPixelCoords(const Mat& bright_channel_img) {
	std::multimap<uchar, std::pair<int, int>> top_bright_values_to_coord;

	int height = bright_channel_img.rows;
	int width = bright_channel_img.cols;

	int max_values_considered = ((double)height * width) * ratio_of_bright_pixels_;
	int curr_values_considered = 0;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			uchar bright_value = bright_channel_img.at<uchar>(i, j);

			if (curr_values_considered < max_values_considered) {
				top_bright_values_to_coord.insert(std::make_pair(bright_value, std::make_pair(i, j)));
				curr_values_considered++;
			}
			else {
				top_bright_values_to_coord.erase(top_bright_values_to_coord.cbegin());
				top_bright_values_to_coord.insert(std::make_pair(bright_value, std::make_pair(i, j)));
			}
		}
	}

	std::vector<std::pair<int, int>> res;

	for (auto it = top_bright_values_to_coord.cbegin(); it != top_bright_values_to_coord.end(); it++) {
		res.push_back(it->second);
	}

	return res;
}

Vec3b AtmosphericLightEstimator::getMeanOfSelectedPixels(const Mat& img, std::vector<std::pair<int, int>> selected_coords) {
	long long sum0 = 0, sum1 = 0, sum2 = 0;

	for (const std::pair<int, int>& coord : selected_coords) {
		sum0 += img.at<Vec3b>(coord.first, coord.second)[0];
		sum1 += img.at<Vec3b>(coord.first, coord.second)[1];
		sum2 += img.at<Vec3b>(coord.first, coord.second)[2];
	}

	return Vec3b(sum0 / selected_coords.size(), sum1 / selected_coords.size(), sum2 / selected_coords.size());
}

