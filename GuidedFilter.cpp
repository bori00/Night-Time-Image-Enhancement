#include "stdafx.h"
#include "GuidedFilter.h"

Mat GuidedFilter::guide(Mat p, Mat I, int r, double epsilon)
{
	// based on wikipedia: https://en.wikipedia.org/wiki/Guided_filter
	int w = 2 * r + 1;
	Size window_size = Size(w, w);

	// step 1
	Mat meanI, meanP, corrI, corrIp;
	blur(I, meanI, window_size);
	blur(p, meanP, window_size);
	blur(I.mul(I), corrI, window_size);
	blur(I.mul(p), corrIp, window_size);

	// step 2
	Mat varI, covIp;
	varI = corrI - meanI.mul(meanI);
	covIp = corrIp - meanI.mul(meanP);

	// step 3
	Mat a, b;
	divide(covIp, varI + epsilon, a);
	b = meanP - a.mul(meanI);

	// step 4
	Mat meanA, meanB;
	blur(a, meanA, window_size);
	blur(b, meanB, window_size);

	// step 5
	Mat q = meanA.mul(I) + meanB;

	return q;
}
