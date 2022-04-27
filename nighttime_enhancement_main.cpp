// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"

#include "PatchOrderedFilter.h"
#include "AtmosphericLightEstimator.h"
#include "TransmissionEstimator.h"
#include "ImageReconstructor.h"


/* ---------------- Parameters ------------------*/

// patch size = (2 * PATCH_RADIUS + 1) x (2 * PATC_RADIUS + 1)
// the higher the patch size, the longer the computation time, 
// but the brighter and clearer the final image
const int PATCH_RADIUS = 5;

const bool COMPUTE_BRUTE_FORCE_CHANNELS = 0;

const double ALPHA = 0.4; // for dark-channel-based transmission estimate correction
const double EPSYLON = 1e3; // for the guided filtering


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

		Mat bright_channel_img = DynamicPatchOrderedFilter().getFilteredImg(img, PATCH_RADIUS, &max_filter);

		Mat dark_channel_img = DynamicPatchOrderedFilter().getFilteredImg(img, PATCH_RADIUS, &min_filter);

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time (Dynamic) = %.3f [ms]\n", t * 1000);

		// todo: remove entire if. Used for testing only
		if (COMPUTE_BRUTE_FORCE_CHANNELS) {

			t = (double)getTickCount(); // Get the current time [s]

			Mat brute_bright_channel_img = BruteForcePatchOrderedFilter().getFilteredImg(img, PATCH_RADIUS, &max_filter);

			Mat brute_dark_channel_img = BruteForcePatchOrderedFilter().getFilteredImg(img, PATCH_RADIUS, &min_filter);

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

		// 3. enhance using both channels and guided filtering
		GuidedFilteringDoubleChannelTransmissionEstimator t_estimator3(ALPHA, EPSYLON, PATCH_RADIUS * 2 + 1, img);

		Mat t_estimate3 = t_estimator3.getEstimation(bright_channel_img, dark_channel_img, atm_light);

		imshow("guided filtering double channel t estimation", t_estimate3);

		Mat imgJ3 = ImageReconstructor().reconstruct(img, atm_light, t_estimate3);

		imshow("Guided Double-Channel Enhanced Image", imgJ3);

		// comparison

		/*Mat imgComp;
		cv::absdiff(imgJ1, imgJ2, imgComp);

		imshow("Difference", imgComp);*/

		// simply normalized image

		/*Mat imgNorm;
		cv::normalize(img, imgNorm, 0, 255, cv::NORM_MINMAX);

		imshow("Simply Normalized Image", imgNorm);

		waitKey();*/
	}
	return 0;
}