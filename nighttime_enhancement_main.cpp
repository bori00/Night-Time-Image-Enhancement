// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"

#include "PatchFilter.h"


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
	while (openFileDlg(fname))
	{
		Mat img = imread(fname, IMREAD_COLOR);

		Mat bright_channel_img = getBrightChannelImage(img);

		Mat dark_channel_img = getDarkChannelImage(img);

		imshow("Original Image", img);

		imshow("Bright Channel", bright_channel_img);

		imshow("Dark Channel", dark_channel_img);

		waitKey();
	}
	return 0;
}