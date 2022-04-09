// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"

#include "PatchFilter.h"
#include "AtmosphericLightEstimator.h"


/* ---------------- Parameters ------------------*/

// patch size = (2 * PATCH_RADIUS + 1) x (2 * PATC_RADIUS + 1)
// the higher the patch size, the longer the computation time, 
// but the brighter and clearer the final image
const int PATCH_RADIUS = 7;

Mat getBrightChannelImage(const Mat& img) {
	ChannelIntensityMaxFilter max_filter;
	return PatchFilter::getFilteredImg(img, PATCH_RADIUS, &max_filter);
}

Mat getDarkChannelImage(const Mat& img) {
	ChannelIntensityMinFilter min_filter;
	return PatchFilter::getFilteredImg(img, PATCH_RADIUS, &min_filter);
}

int main()
{
	char fname[MAX_PATH];

	AtmosphericLightEstimator atm_light_estimator(0.1);

	while (openFileDlg(fname))
	{
		printf("----------New Image: %s \n", fname);

		Mat img = imread(fname, IMREAD_COLOR);

		Mat bright_channel_img = getBrightChannelImage(img);

		Mat dark_channel_img = getDarkChannelImage(img);

		imshow("Original Image", img);

		imshow("Bright Channel", bright_channel_img);

		imshow("Dark Channel", dark_channel_img);

		uchar atm_light = atm_light_estimator.getEstimate(bright_channel_img);

		printf("Global atmospheric light: %d\n", atm_light);

		waitKey();
	}
	return 0;
}