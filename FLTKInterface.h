#ifndef FLTKINTERFACE_H
#define FLTKINTERFACE_H

#include <FL/Fl_Image.h>

#include "Matrix.h"

namespace FLTKInterface
{
	template<typename T> void matrixToPixelbuffer(const Matrix<T>& src, uchar* dst);
	template<typename T> void matrixToPixelbuffer(const Matrix<T>& srcR, const Matrix<T>& srcG, const Matrix<T>& srcB, uchar* dst);

	template<typename T> void pixelbufferToMatrix(const uchar* src, Matrix<T>& dst);
	template<typename T> void pixelbufferToMatrix(const uchar* src, Matrix<T>& dstR, Matrix<T>& dstG, Matrix<T>& dstB);

	template<typename T> void imageToMatrix(const Fl_Image& src, Matrix<T>& dst);
	template<typename T> void imageToMatrix(const Fl_Image& src, Matrix<T>& dstR, Matrix<T>& dstG, Matrix<T>& dstB);
};

#endif // FLTKINTERFACE_H
