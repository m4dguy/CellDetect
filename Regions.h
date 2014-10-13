#ifndef REGIONS_H
#define REGIONS_H

#include <stack>
#include <vector>

#include "NuLLTools.inl"
#include "NuLLProcessing.inl"

/*
 *
 */

namespace Regions
{
	template <typename T> uint markRegions(const Matrix<T>& mtx, Matrix<T>& dst);
}

#endif // REGIONS_H
