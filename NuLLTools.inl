#ifndef NULLTOOLS_INL
#define NULLTOOLS_INL

#include "NuLLTools.h"

//can be used for matrix conversion
//copy content of matrix src to matrix dst
//useful for type conversion
template <typename T, typename U> void NuLLTools::copyMatrix(const Matrix<T>& src, Matrix<U>& dst)
{
    size_t width = src.width();
    size_t height = src.height();
    for(uint y=0; y<height; ++y)
        for(uint x=0; x<width; ++x)
            dst(x,y) = (U)src(x,y);
}

//copy segment of matrix to dst
//uses absolute coordinates
template <typename T> void NuLLTools::getSegment(const Matrix<T>& src, Matrix<T>& dst, int startX, int startY, int endX, int endY)
{

    if(endX < startX)
        std::swap(startX, endX);

    if(endY < startY)
        std::swap(startY, endY);

    for(int y=startY; y<endY; ++y)
        for(int x=startX; x<endX; ++x)
            dst(x,y) = src.getMirrored(x,y);
}

//pastes matrix "other" into matrix "mtx"
template <typename T> void NuLLTools::pasteAt(const Matrix<T>& mtx, Matrix<T>& dst, int dstX, int dstY)
{
	size_t width = mtx.width();
	size_t height = mtx.height();
	for(uint y=0; y<width; ++y)
		for(uint x=0; x<height; ++x)
			dst(x+dstX, y+dstY) = mtx(x,y);
}

//copy matrix column to vector
template <typename T> void NuLLTools::getColumn(const Matrix<T>& src, Vector<T>& dst, uint col)
{
    size_t dim = src.height();
    for(uint y=0; y<dim; ++y)
        dst[y] = src(col, y);

}

//copy matrix row to vector
template <typename T> void NuLLTools::getRow(const Matrix<T>& src, Vector<T>& dst, uint row)
{
	size_t dim = src.width();
	for(uint x=0; x<dim; ++x)
		dst[x] = src(x, row);

}

template <typename T> void NuLLTools::elementwiseMultiplication(const Matrix<T>& mtx1, const Matrix<T> mtx2, Matrix<T>& dst)
{
	size_t width = mtx1.width();
	size_t height = mtx1.height();

	for(uint y=0; y<height; ++y)
		for(uint x=0; x<width; ++x)
			dst(x,y) = mtx1(x,y) * mtx2(x,y);
}

template <typename T> void NuLLTools::elementwiseAddition(const Matrix<T>& mtx1, const Matrix<T> mtx2, Matrix<T>& dst)
{
	size_t width = mtx1.width();
	size_t height = mtx1.height();

	for(uint y=0; y<height; ++y)
		for(uint x=0; x<width; ++x)
			dst(x,y) = mtx1(x,y) + mtx2(x,y);
}

template <typename T> void NuLLTools::elementwiseEquals(const Matrix<T>& mtx1, const Matrix<T> mtx2, Matrix<T>& dst)
{
	size_t width = mtx.width();
	size_t height = mtx.height();

	for(uint y=0; y<height; ++y)
		for(uint x=0; x<width; ++x)
			dst(x,y) = (mtx1(x,y) == mtx2(x,y));
}

template <typename T> std::pair<T,T> NuLLTools::minMaxValue(const Matrix<T>& mtx)
{
	T minV, maxV;
	maxV = minV = mtx(0,0);
	size_t width = mtx.width();
	size_t height = mtx.height();

	for(uint y=0; y<height; ++y)
	{
		for(uint x=0; x<width; ++x)
		{
			minV = std::min(minV, mtx(x,y));
			maxV = std::max(maxV, mtx(x,y));
		}
	}


	return std::make_pair(minV, maxV);
}

template <typename T> T NuLLTools::maxValue(const Matrix<T>& mtx)
{
	return minMaxValue(mtx).second;
}

template <typename T> T NuLLTools::minValue(const Matrix<T>& mtx)
{
	return minMaxValue(mtx).first;
}

template <typename T> T NuLLTools::mean(const Matrix<T>& mtx)
{
	T avg = 0;
	size_t width = mtx.width();
	size_t height = mtx.height();

	for(uint y=0; y<height; ++y)
		for(uint x=0; x<width; ++x)
			avg += mtx(x,y);

	avg /= (T)(width * height);
	return avg;
}

template <typename T> T NuLLTools::variance(const Matrix<T>& mtx)
{
	T variance = 0;
	size_t width = mtx.width();
	size_t height = mtx.height();

	T mean = NuLLTools::mean(mtx);

	for(uint y=0; y<height; ++y)
		for(uint x=0; x<width; ++x)
			variance += (mtx(x,y) - mean) * (mtx(x,y) - mean);

	variance /= ((width * height)-1);
	return (T)sqrt((double)variance);
}

template <typename T> void NuLLTools::mAbs(const Matrix<T>& mtx, Matrix<T>& dst)
{
	size_t width = mtx.width();
	size_t height = mtx.height();

	for(uint y=0; y<height; ++y)
		for(uint x=0; x<width; ++x)
			dst(x,y) = (mtx(x,y)>=0)? mtx(x,y) : ((-1)*mtx(x,y));
}

template <typename T> void NuLLTools::mSqrt(const Matrix<T>& mtx, Matrix<T>& dst)
{
	size_t width = mtx.width();
	size_t height = mtx.height();

	for(uint y=0; y<height; ++y)
		for(uint x=0; x<width; ++x)
			dst(x,y) = (T)sqrt((double)mtx(x,y));
}

template<typename T> void NuLLTools::imageHistogram(const Matrix<T>& src, Vector<T>& dst)
{
	size_t width = src.width();
	size_t height= src.height();

	dst.resize(256);
	dst.fill(0);

	for(uint y=0; y<height; ++y)
		for(uint x=0; x<width; ++x)
			++dst[(uint)src(x,y)];
}


#endif
