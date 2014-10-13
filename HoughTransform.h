#ifndef HOUGHTRANSFORM_H
#define HOUGHTRANSFORM_H

#include <vector>
#include <set>

#include "Vector.h"
#include "Matrix.h"

namespace HoughTransform
{
	typedef struct
	{
		uint ID;
		uint x;
		uint y;
		uint radius;
		uint area;
		uint marker;
		double score;
	}HoughCircle;

	template <typename T> void houghCircles(const Matrix<T>& img, std::vector<HoughCircle>& circles, double threshold=100, int minRad=1, int maxRad=50);
	void mergeCircles(std::vector<HoughCircle>& circles, uint maxDist=10);

	template <typename T> void plotCircles(const std::vector<HoughCircle>& circles, Matrix<T>& img, uint radius=1);
	template <typename T> void plotCirclesFilled(const std::vector<HoughCircle>& circles, Matrix<T>& img);
	
	void changeRadius(std::vector<HoughCircle>& circles, int add=3);
	template <typename T> double averageInCircle(const Matrix<T>& mtx, const HoughCircle& circ, int border=0);
	template <typename T> double averageInNeighbourhood(const Matrix<T>& mtx, const Matrix<T>& mask, const HoughCircle& circ, uint radius=5);
	template <typename T> double varianceInCircle(const Matrix<T>& mtx, const HoughCircle& circ, int border=0);
	
	template <typename T> int pixelValueInCircle(const Matrix<T>& mtx, const HoughCircle& circ, int value);
	template <typename T> int foregroundPixelsInCircle(const Matrix<T>& mtx, const HoughCircle& circ);
	template <typename T> int uniqueInCircle(const Matrix<T>& mtx, const HoughCircle& circ);

	template <typename T> void histogramInCircle(const Matrix<T>& mtx, const HoughCircle& circ, Vector<T>& hist);
};

#endif	//HOUGHTRANSFORM_H
