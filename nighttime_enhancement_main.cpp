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
const int PATCH_RADIUS = 7;
// for dark-channel-based transmission estimate correction
const double ALPHA = 0.4;
// for the guided filtering
const double EPSYLON = 1e3;


int main()
{
	char fname[MAX_PATH];

	while (openFileDlg(fname))
	{
		// --------------- 0th step. Input
		Mat img = imread(fname, IMREAD_COLOR);
		printf("----------New Image: %s \n", fname);

		printf("The name of the output folder:");
		std::string output_foldername;
		std::cin >> output_foldername;

		imshow("Original Image", img);
		imwrite(output_foldername + "\\OriginalImage.bmp", img);

		// --------------- 1st step. Bright- and dark-channel computation.
		ChannelIntensityMaxFilter max_filter;
		Mat bright_channel_img = DynamicPatchOrderedFilter().getFilteredImg(img, PATCH_RADIUS, &max_filter);
		imshow("Bright Channel", bright_channel_img);
		imwrite(output_foldername + "\\BrightChannel.bmp", bright_channel_img);

		ChannelIntensityMinFilter min_filter;
		Mat dark_channel_img = DynamicPatchOrderedFilter().getFilteredImg(img, PATCH_RADIUS, &min_filter);
		imshow("Dark Channel", dark_channel_img);
		imwrite(output_foldername + "\\DarkChannel.bmp", dark_channel_img);

		// --------------- 2nd step. Atmospheric light estimation
		AtmosphericLightEstimator atm_light_estimator(0.1);
		Vec3b atm_light = atm_light_estimator.getEstimate(img, bright_channel_img);
		
		// --------------- 3rd step. Estimating the transmission T based on the bright channel only.
		BrightOnlyTransmissionEstimator t_estimator1;
		Mat t_estimate1 = t_estimator1.getEstimation(bright_channel_img, dark_channel_img, atm_light);
		imshow("bright channel t estimation", t_estimate1);
		imwrite(output_foldername + "\\BrightChannelTEstimate.bmp", t_estimate1 * 255);

		Mat imgJ1 = ImageReconstructor().reconstruct(img, atm_light, t_estimate1);
		imshow("Bright-Only Enhanced Image", imgJ1);
		imwrite(output_foldername + "\\BrightChannelEnhancedImage.bmp", imgJ1);

		// --------------- 4th step. Estimating the transmission T based on both the bright and the dark channel.
		DoubleChannelTransmissionEstimator t_estimator2(ALPHA);
		Mat t_estimate2 = t_estimator2.getEstimation(bright_channel_img, dark_channel_img, atm_light);
		imshow("double channel t estimation", t_estimate2);
		imwrite(output_foldername + "\\DoubleChannelTEstimate.bmp", t_estimate2 * 255);

		Mat imgJ2 = ImageReconstructor().reconstruct(img, atm_light, t_estimate2);
		imshow("Double-Channel Enhanced Image", imgJ2);
		imwrite(output_foldername + "\\DoubleChannelEnhancedImage.bmp", imgJ2);

		// --------------- 5th step. Refining the transmission map using a guided filter.
		GuidedFilteringDoubleChannelTransmissionEstimator t_estimator3(ALPHA, EPSYLON, PATCH_RADIUS * 2 + 1, img);
		Mat t_estimate3 = t_estimator3.getEstimation(bright_channel_img, dark_channel_img, atm_light);
		imshow("guided filtering double channel t estimation", t_estimate3);
		imwrite(output_foldername + "\\GuidedDoubleChannelTEstimate.bmp", t_estimate3 * 255);

		// --------------- 6th step. Obtaining the well-exposed image based on the refined transmission estimation.
		Mat imgJ3 = ImageReconstructor().reconstruct(img, atm_light, t_estimate3);
		imshow("Guided Double-Channel Enhanced Image", imgJ3);
		imwrite(output_foldername + "\\GuidedDoubleChannelEnhancedImage.bmp", imgJ3);
	}
	return 0;
}