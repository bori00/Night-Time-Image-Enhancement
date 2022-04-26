// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"

#include "PatchFilter.h"
#include "AtmosphericLightEstimator.h"
#include "TransmissionEstimator.h"
#include "ImageReconstructor.h"


/* ---------------- Parameters ------------------*/

// patch size = (2 * PATCH_RADIUS + 1) x (2 * PATC_RADIUS + 1)
// the higher the patch size, the longer the computation time, 
// but the brighter and clearer the final image
const int PATCH_RADIUS = 5;

const bool COMPUTE_BRUTE_FORCE_CHANNELS = 0;

const double ALPHA = 0.4;


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

		Vec3b atm_light = atm_light_estimator.getEstimate(img, bright_channel_img);

		printf("Global atmospheric light: (%d, %d, %d)\n", atm_light[0], atm_light[1], atm_light[2]);


		// 1. enhance using bright channel only
		BrightOnlyTransmissionEstimator t_estimator1;

		Mat t_estimate1 = t_estimator1.getEstimation(bright_channel_img, dark_channel_img, atm_light);

		imshow("bright channel t estimation", t_estimate1);

		Mat imgJ1 = ImageReconstructor().reconstruct(img, atm_light, t_estimate1);

		imshow("Bright-Only Enhanced Image", imgJ1);

		// 2. enhance using both channels
		DoubleChannelTransmissionEstimator t_estimator2(ALPHA);

		Mat t_estimate2 = t_estimator2.getEstimation(bright_channel_img, dark_channel_img, atm_light);

		imshow("double channel t estimation", t_estimate2);

		Mat imgJ2 = ImageReconstructor().reconstruct(img, atm_light, t_estimate2);

		imshow("Double-Channel Enhanced Image", imgJ2);

		// comparison

		Mat imgComp;
		cv::absdiff(imgJ1, imgJ2, imgComp);

		// cv::normalize(imgComp, imgComp, 0, 255, cv::NORM_MINMAX);

		imshow("Difference", imgComp);

		waitKey();
	}
	return 0;
}