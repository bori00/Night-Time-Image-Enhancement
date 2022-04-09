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

const bool COMPUTE_BRUTE_FORCE_CHANNELS = 1;


int main()
{
	char fname[MAX_PATH];

	AtmosphericLightEstimator atm_light_estimator(0.1);

	ChannelIntensityMaxFilter max_filter;

	ChannelIntensityMinFilter min_filter;

	while (openFileDlg(fname))
	{
		printf("----------New Image: %s \n", fname);

		Mat img = imread(fname, IMREAD_COLOR);

		double t = (double)getTickCount(); // Get the current time [s]

		Mat bright_channel_img = DynamicPatchFilter().getFilteredImg(img, PATCH_RADIUS, &max_filter);

		Mat dark_channel_img = DynamicPatchFilter().getFilteredImg(img, PATCH_RADIUS, &min_filter);

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time (Dynamic) = %.3f [ms]\n", t * 1000);


		// todo: remove entire if. Used for testing only
		if (COMPUTE_BRUTE_FORCE_CHANNELS) {

			t = (double)getTickCount(); // Get the current time [s]

			Mat brute_bright_channel_img = BruteForcePatchFilter().getFilteredImg(img, PATCH_RADIUS, &max_filter);

			Mat brute_dark_channel_img = BruteForcePatchFilter().getFilteredImg(img, PATCH_RADIUS, &min_filter);

			// Get the current time again and compute the time difference [s]
			t = ((double)getTickCount() - t) / getTickFrequency();
			// Print (in the console window) the processing time in [ms] 
			printf("Time (Brute Force) = %.3f [ms]\n", t * 1000);

			bool eq_bright = cv::countNonZero(bright_channel_img != brute_bright_channel_img) == 0;

			bool eq_dark = cv::countNonZero(dark_channel_img != brute_dark_channel_img) == 0;

			printf("The equality of bright=%d and dark=%d\n", eq_bright, eq_dark);

			assert(eq_bright && eq_dark);
		}

		imshow("Original Image", img);

		imshow("Bright Channel", bright_channel_img);

		imshow("Dark Channel", dark_channel_img);

		uchar atm_light = atm_light_estimator.getEstimate(bright_channel_img);

		printf("Global atmospheric light: %d\n", atm_light);

		waitKey();
	}
	return 0;
}