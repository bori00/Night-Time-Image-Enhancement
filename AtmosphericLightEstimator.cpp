#include "stdafx.h"
#include "AtmosphericLightEstimator.h"

AtmosphericLightEstimator::AtmosphericLightEstimator(double ratio_of_bright_pixels)
	: ratio_of_bright_pixels_(ratio_of_bright_pixels) {}

uchar AtmosphericLightEstimator::getEstimate(const Mat& bright_channel_img) {
	std::map<uchar, int> top_bright_values_to_freq;

	int height = bright_channel_img.rows;
	int width = bright_channel_img.cols;

	int max_values_considered = ((double)height * width) * ratio_of_bright_pixels_;
	int curr_values_considered = 0;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			uchar bright_value = bright_channel_img.at<uchar>(i, j);

			if (curr_values_considered < max_values_considered) {
				addBrightValueToTop(top_bright_values_to_freq, bright_value);
				curr_values_considered++;
			}
			else {
				if ((*top_bright_values_to_freq.cbegin()).first < bright_value) {
					removeLowestTopBrightValue(top_bright_values_to_freq);
					addBrightValueToTop(top_bright_values_to_freq, bright_value);
				}
			}
		}
	}

	return getWeightedMean(top_bright_values_to_freq);
}

void AtmosphericLightEstimator::removeLowestTopBrightValue(std::map<uchar, int>& values_to_freq) {
	if ((*values_to_freq.cbegin()).second > 1) {
		values_to_freq.begin()->second--;
	}
	else {
		values_to_freq.erase(values_to_freq.begin()->first);
	}
}

void AtmosphericLightEstimator::addBrightValueToTop(std::map<uchar, int>& values_to_freq, uchar value) {
	if (values_to_freq.find(value) != values_to_freq.end()) {
		values_to_freq[value]++;
	}
	else {
		values_to_freq[value] = 1;
	}
}

uchar AtmosphericLightEstimator::getWeightedMean(const std::map<uchar, int>& values_to_freq) {
	long long sum = 0;
	int nr_instances = 0;

	for (auto it = values_to_freq.cbegin(); it != values_to_freq.cend(); ++it) {
		sum += it->first * it->second;
		nr_instances += it->second;
	}

	return sum / nr_instances;
}