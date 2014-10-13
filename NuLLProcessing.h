#ifndef NULLPROCESSING_H
#define NULLPROCESSING_H

#include <algorithm>
#include <vector>
#include <math.h>

#include "NuLLTools.inl"

/*
 *
 * methods for data and signal processing
 * includes:    point operations
 *              morphological filters
 *              derivative filters (first order, second order)
 *              kernel generators
 *              convolution
 *				scaling
 *
 */

namespace NuLLProcessing
{
	//kernel operations
    template <typename T> void normalize(Matrix<T>& mtx);
    template <typename T> void convolve(const Matrix<T>& mtx, Matrix<T>& dst, const Matrix<T>& kernel);
	template <typename T> void convolve(const Matrix<T>& mtx, Matrix<T>& dst, const Vector<T>& kernel);
	template <typename T> void convolve(const Matrix<T>& mtx, Matrix<T>& dst, const Vector<T>& kernelX, const Vector<T>& kernelY);
	template <typename T> void convolveX(const Matrix<T>& mtx, Matrix<T>& dst, const Vector<T>& kernelX);
	template <typename T> void convolveY(const Matrix<T>& mtx, Matrix<T>& dst, const Vector<T>& kernelY);

	//kernel generators
	template <typename T> void identityKernel(Matrix<T>& dst, int radius = 1);
	template <typename T> void pillboxKernel(Matrix<T>& dst, int radius = 1);
    template <typename T> void gaussianKernel(Matrix<T>& dst, int radius = 1, double sigma = 0.0);

	//simple blur techniques
	template <typename T> void boxBlur(const Matrix<T>& mtx, Matrix<T>& dst, int radius = 1);
	template <typename T> void gaussianBlur(const Matrix<T>& mtx, Matrix<T>& dst, int radius = 1, double sigma = 0.0);

	template <typename T> void differenceOfGaussians(const Matrix<T>& mtx, Matrix<T>& dst, int sigma1 = 0.0, int sigma2 = 10.0);

	//derivative filters
	template <typename T> void firstDerivative(const Matrix<T>& mtx, Matrix<T>& dst);
	template <typename T> void firstDerivative(const Matrix<T>& mtx, Matrix<T>& dstGrad, Matrix<T>& dstDir);
	template <typename T> void secondDerivative(const Matrix<T>& mtx, Matrix<T>& dst);
	template <typename T> void laplacian(const Matrix<T>& mtx, Matrix<T>& dst);

	//others
	template <typename T> void distanceTransform(const Matrix<T>& mtx, Matrix<T>& dst);
	template <typename T> void localVariance(const Matrix<T>& mtx, Matrix<T>& dst, int radius = 1);
	template <typename T> void meanCurvature(const Matrix<T>& mtx, Matrix<T>& dst);
	template <typename T> void harrisCorners(const Matrix<T>& mtx, Matrix<T>& dst);

	//morphological filters
    template <typename T> void medianFilter(const Matrix<T>& mtx, Matrix<T>& dst, int radius = 1, float percentile = .5f);
    template <typename T> void dilation(const Matrix<T>& mtx, Matrix<T>& dst, int radius = 1);
    template <typename T> void erosion(const Matrix<T>& mtx, Matrix<T>& dst, int radius = 1);
    template <typename T> void opening(const Matrix<T>& mtx, Matrix<T>& dst, int radius = 1);
    template <typename T> void closing(const Matrix<T>& mtx, Matrix<T>& dst, int radius = 1);
	template <typename T> void whiteTopHat(const Matrix<T>& mtx, Matrix<T>& dst, int radius = 1);
	template <typename T> void blackTopHat(const Matrix<T>& mtx, Matrix<T>& dst, int radius = 1);
	template <typename T> void selfdualTopHat(const Matrix<T>& mtx, Matrix<T>& dst, int radius = 1);

	//point operations
	template <typename T> void gammaCorrection(const Matrix<T>& mtx, Matrix<T>& dst, double gamma = 1.2);
	template <typename T> void logDynamicCompression(const Matrix<T>& mtx, Matrix<T>& dst, double c = 0);
	template <typename T> void affineRescale(const Matrix<T>& mtx, Matrix<T>& dst, double minVal = 0, double maxVal = 255);
	template <typename T> void affineTransform(const Matrix<T>& mtx, Matrix<T>& dst, double a = 1, double b = 0);

	template <typename T> void snap(const Matrix<T>& mtx, Matrix<T>& dst, T min = 0, T max = 255);
	template <typename T> void thresholding(const Matrix<T>& mtx, Matrix<T>& dst, double threshold, double gmin = 0, double gmax = 255);
	template <typename T> void doubleThresholding(const Matrix<T>& mtx, Matrix<T>& dst, double thresholdLower, double thresholdUpper, double gmin = 0, double gmax = 255);
	template <typename T> void automatedThresholding(const Matrix<T>& mtx, Matrix<T>& dst, double gmin = 0, double gmax = 255);

	//up- and downscaling
	template <typename T> void downsample(const Matrix<T>& mtx, Matrix<T>& dst, uint factorX=2, uint factorY=0);
	template <typename T> void upsample(const Matrix<T>& mtx, Matrix<T>& dst, uint factor=2, uint factorY=0);

	//mirroring
	template <typename T> void mirrorX(const Matrix<T>& mtx, Matrix<T>& dst);
	template <typename T> void mirrorY(const Matrix<T>& mtx, Matrix<T>& dst);

	//misc/experimental
	template <typename T> void statisticalRegionMerging(const Matrix<T>& mtx, Matrix<T>& dst, T threshold=5.);
}

#endif // NULLPROCESSING_H
