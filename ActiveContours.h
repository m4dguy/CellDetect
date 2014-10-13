#ifndef ACTIVECONTOURS_H
#define ACTIVECONTOURS_H

#include "Matrix.h"
#include "NuLLProcessing.inl"

#define PI 3.14159265

namespace ActiveContours
{
	const int circles = 0;
	const int bigCircle = 1;
	const int square = 2;

	struct Statistics
	{
		double averageIn;
		double averageOut;
		
		uint area;
		uint length;
	};
	
	//for score calculation
	template<typename T> void updateStatistics(const Matrix<T>& lvlSet, const Matrix<T>& img, Statistics& stats);
	template<typename T> void updateProbabilities(const Matrix<T>& lvlSet, const Matrix<T>& img, Vector<double>& logProbsIn, Vector<double>& logProbsOut);
	template<typename T> void spf(const Matrix<T>& lvlSet, const Matrix<T>& img, Matrix<T>& dst);

	//initialization
	template<typename T> void initializeLevelSet(Matrix<T>& ini);
	template<typename T> void initializeEdgeStoppingFunction(const Matrix<T>& src, Matrix<T>& edgeStop, const double epsilon=.1);

	//initial shapes for segmentation
	template<typename T> void initializeForeground(Matrix<T>& ini, const int type=1);
	
	//reinitliazation of level set
	template<typename T> void reinitialize(const Matrix<T>& src, Matrix<T>& dst);

	//heaviside functions
	template<typename T> void regularisedHeaviside(const Matrix<T>& src, Matrix<T>& dst);
	template<typename T> void heaviside(const Matrix<T>& src, Matrix<T>& dst);

	//segmentation
	template<typename T> void regionBased(const Matrix<T>& src, Matrix<T>& dst, const double mu=1., const double nu=1., const double lambda1=1., const double lambda2=1., const double tau=.1, const double epsilon=.1, const uint reini=50, const uint iterations=100);
	template<typename T> void edgeBased(const Matrix<T>& src, Matrix<T>& dst, const double tau=.1, const double alpha=5., const double epsilon=.1, const uint reini=30, const uint iterations=500);
	template<typename T> void pdfBased(const Matrix<T> & src, Matrix<T>& dst, const double nu=1., const double epsilon=.1, const double tau=.1, const uint reini=30, const uint iterations=500);
	template<typename T> void spfBased(const Matrix<T>& src, Matrix<T>& dst, const double alpha=0., const double tau=.1, const uint iterations=500);
}

#endif