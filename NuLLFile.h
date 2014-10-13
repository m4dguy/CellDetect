#ifndef NULLFILE_H
#define NULLFILE_H

#include <fstream>

#include "Matrix.h"

/*
 * Multiple file dumps
 *
 */

namespace NuLLFile
{
	template<typename T> int writeCSV(const Matrix<T>& src, const char* filename);

	template<typename T> int writePGM(const Matrix<T>& src, const char* filename);
	template<typename T> int writePPM(const Matrix<T>& srcR, const Matrix<T>& srcG, const Matrix<T>& srcB, const char* filename);
};

#endif // NULLFILE_H
