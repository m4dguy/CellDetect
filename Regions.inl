#ifndef REGIONS_INL
#define REGIONS_INL

#include "Regions.h"

//mark regions in binary input image
//breadth-first search-like approach
template <typename T> inline uint Regions::markRegions(const Matrix<T>& mtx, Matrix<T>& dst)
{
	dst.fill(0);
	uint ID = 0;

	size_t width = mtx.width();
	size_t height = mtx.height();

	std::stack<std::pair<uint, uint> > next;

	//find candidate points
	for(uint y=1; y<height-1; ++y)
	{
		for(uint x=1; x<width-1; ++x)
		{
			if(mtx(x,y) && !dst(x,y))
			{
				dst(x,y) = ++ID;
				next.push(std::make_pair(x, y));

				while(!next.empty())
				{
					std::pair<uint, uint> pnext = next.top();
					next.pop();
					uint nx = pnext.first;
					uint ny = pnext.second;

					dst(nx, ny) = ID;

					for(int rx=-1; rx<=1; ++rx)
					{
						if((nx+rx<0) ||(nx+rx>=width))
							continue;

						if(mtx(nx+rx, ny) && !dst(nx+rx, ny))
						{
							next.push(std::make_pair(nx+rx, ny));
						}
					}

					for(int ry=-1; ry<=1; ++ry)
					{
						if((ny+ry<0) ||(ny+ry>=height))
							continue;
						
						if(mtx(nx, ny+ry) && !dst(nx, ny+ry))
						{
							next.push(std::make_pair(nx, ny+ry));
						}
					}
				}
			}
		}
	}
	return ID;
}

#endif
