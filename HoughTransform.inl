#ifndef HOUGHTRANSFORM_INL
#define HOUGHTRANSFORM_INL

#include "HoughTransform.h"

template <typename T> inline void HoughTransform::houghCircles(const Matrix<T>& img, std::vector<HoughCircle>& circles, double threshold, int minRad, int maxRad)
{
	HoughCircle circ;
	int rx, ry;								//radii in x and y direction
	uint area;								//area of currrent circular area
	uint pixels;							//number of pixels in current mask
	double score;							//overall score
	double sq, radSq, radSqLow, radSqUp;		//precalculated squared radii

	const uint width = img.width();
	const uint height = img.height();

	std::vector<std::pair<int,int> > mask;
	//mask.reserve((maxRad * maxRad) - ((maxRad-2) * (maxRad-2)));
	mask.reserve(4*maxRad);

	for(int rad=minRad; rad<=maxRad; ++rad)
	{
		area = 0;
		sq = rad * rad;
		radSqLow = (rad - .5) * (rad - .5);
		radSqUp = (rad + .5) * (rad + .5);

		//precalculate pixels in circle
		for(rx=-rad; rx<=rad; ++rx)
		{
			for(ry=-rad; ry<=rad; ++ry)
			{
				radSq = (rx*rx + ry*ry);

				//determine area
				if(radSq <=  sq)
					++area;

				//identify border pixels
				if((radSqLow <= radSq) && (radSq < radSqUp))
				{
					mask.push_back(std::make_pair(rx,ry));
				}
			}
		}

		//get circle length
		pixels = mask.size();

		//check image
		for(uint y=rad; y<height-rad; ++y)
		{
			for(uint x=rad; x<width-rad; ++x)
			{
				score = 0;
				for(uint i=0; i<pixels; ++i)
				{
					rx = mask[i].first;
					ry = mask[i].second;
					score += img(x+rx, y+ry);
				}
				
				//check if circle found
				score /= pixels;
				if(score >= threshold)
				{
					circ.ID = 255;
					circ.x = x;
					circ.y = y;
					circ.radius = rad;
					circ.area = area;
					circ.score = score;
					circles.push_back(circ);
				}
			}
		}
		mask.clear();
	}
}

template <typename T> inline void HoughTransform::plotCircles(const std::vector<HoughCircle>& circles, Matrix<T>& img, uint radius)
{
	uint x, y;
	int rad, radSq;
	double radSqLow, radSqUp;

	const uint width = img.width();
	const uint height = img.height();

	for(uint i=0; i<circles.size(); ++i)
	{
		for(uint r=0; r<radius; ++r)
		{
			HoughCircle c = circles[i];
			x = c.x;
			y = c.y;
			rad = c.radius - r;
			radSqLow = (rad - .5) * (rad - .5);
			radSqUp = (rad + .5) * (rad + .5);

			for(int ry=-rad; ry<=rad; ++ry)
			{
				for(int rx=-rad; rx<=rad; ++rx)
				{
					radSq = (rx*rx + ry*ry);
					if((radSqLow <= radSq) && (radSq < radSqUp))
					{
						img(x+rx, y+ry) = c.ID;
					}
					
				}
			}
		}
	}
}

template <typename T> inline void HoughTransform::plotCirclesFilled(const std::vector<HoughCircle>& circles, Matrix<T>& img)
{
	uint x, y;
	int rad, radSq;
	const uint width = img.width();
	const uint height = img.height();

	for(uint i=0; i<circles.size(); ++i)
	{
		HoughCircle c = circles[i];
		x = c.x;
		y = c.y;
		rad = c.radius;
		radSq = rad * rad;

		for(int ry=-rad; ry<=rad; ++ry)
		{
			for(int rx=-rad; rx<=rad; ++rx)
			{
				if((rx*rx + ry*ry) <= radSq)
				{
					img(x+rx, y+ry) = 255;
				}
			}
		}
	}
}

void inline HoughTransform::changeRadius(std::vector<HoughCircle>& circles, int add)
{
	for(uint i=0; i<circles.size(); ++i)
		circles[i].radius += add;
}

//merge nearby circles
void inline HoughTransform::mergeCircles(std::vector<HoughCircle>& circles, uint maxDist)
{
	bool changed = 1;								//marker if circles have been merged

	uint dist;										//current distance
	uint bSize;										//size of batch
	const uint maxDistSq = maxDist * maxDist;

	HoughCircle c;

	std::vector<uint> merged;						//markers identifying circles for merging
	std::vector<HoughCircle> batch;					//batch containg all circles for merging
	std::vector<HoughCircle> res;					//resulting vector
	merged.resize(circles.size());
	batch.reserve(circles.size());
	res.reserve(circles.size());

	//iteratively merge circles
	while(changed)
	{
		changed = 0;
		res.clear();
		merged.clear();
		merged.resize(circles.size());
		for(uint i=0; i<circles.size()-1; ++i)
		{
			if(merged[i])
				continue;

			for(uint j=i+1; j<circles.size(); ++j)
			{
				if(merged[j])
					continue;

				dist = ((circles[i].x - circles[j].x) * (circles[i].x - circles[j].x)) + ((circles[i].y - circles[j].y) * (circles[i].y - circles[j].y));
				if(dist <= maxDistSq)
				{
					merged[i] = 1;
					merged[j] = 1;
					changed = 1;
					batch.push_back(circles[j]);
				}
			}

			bSize = batch.size();
			if(bSize)
			{
				c = batch[0];
				for(uint b=0; b<bSize; ++b)
				{
					if(batch[b].score > c.score)
						c = batch[b];
				}
				res.push_back(c);
				batch.clear();
			}
		}

		for(uint i=0; i<circles.size(); ++i)
		{
			if(!merged[i])
			{
				res.push_back(circles[i]);
			}
		}
		res.swap(circles);
	}
}

template <typename T> inline double HoughTransform::averageInCircle(const Matrix<T>& mtx, const HoughCircle& circ, int border)
{
	double intensity = 0;
	double area = 0;
	uint x = circ.x;
	uint y = circ.y;
	int radius = circ.radius - border;
	int radSq = radius * radius;

	for(int ry=-radius; ry<=radius; ++ry)
	{
		for(int rx=-radius; rx<=radius; ++rx)
		{
			if((rx*rx + ry*ry) <= radSq)
			{
				intensity += mtx.getMirrored(x+rx, y+ry);
				++area;
			}
		}
	}
	return (intensity / area);
}

template <typename T> double HoughTransform::averageInNeighbourhood(const Matrix<T>& mtx, const Matrix<T>& mask, const HoughCircle& circ, uint radius)
{
	double intensity = 0;
	double area = 0;
	uint x = circ.x;
	uint y = circ.y;
	int radiusAdd = circ.radius + radius;
	int radSq = radiusAdd * radiusAdd;

	for(int ry=-radiusAdd; ry<=radiusAdd; ++ry)
	{
		for(int rx=-radiusAdd; rx<=radiusAdd; ++rx)
		{
			if((rx*rx + ry*ry) <= radSq)
			{
				if(!mask.getMirrored(x+rx, y+ry))
				{
					intensity += mtx.getMirrored(x+rx, y+ry);
					++area;
				}
			}
		}
	}
	return (intensity / area);
}

template <typename T> inline double HoughTransform::varianceInCircle(const Matrix<T>& mtx, const HoughCircle& circ, int border)
{
	T area = 0;
	uint x = circ.x;
	uint y = circ.y;
	int rad = circ.radius - border;
	int radSq = rad * rad;
	double var = 0;
	double mean = averageInCircle(mtx, circ, border);

	for(int ry=-rad; ry<=rad; ++ry)
	{
		for(int rx=-rad; rx<=rad; ++rx)
		{
			if((rx*rx + ry*ry) <= radSq)
			{
				var += (mtx.getMirrored(x+rx, y+ry) - mean) * (mtx.getMirrored(x+rx, y+ry) - mean);
				++area;
			}
		}
	}
	var /= (area - 1);
	return var;
}

template <typename T> int HoughTransform::pixelValueInCircle(const Matrix<T>& mtx, const HoughCircle& circ, int value)
{
	int res = 0;
	int posX = circ.x;
	int posY = circ.y;
	int radius = circ.radius;

	//avoid casting
	T val = (T)value;

	for(int y=posY-radius; y<=posY+radius; ++y)
	{
		for(int x=posX-radius; x<=posX+radius; ++x)
		{
			if(mtx(x,y) == val)
				++res;
		}
	}
	return res;
}

template <typename T> inline int HoughTransform::foregroundPixelsInCircle(const Matrix<T>& mtx, const HoughCircle& circ)
{
	int res = 0;
	int posX = circ.x;
	int posY = circ.y;
	int radius = circ.radius;

	for(int y=posY-radius; y<=posY+radius; ++y)
	{
		for(int x=posX-radius; x<=posX+radius; ++x)
		{
			if(mtx(x,y))
				++res;
		}
	}
	return res;
}

template <typename T> inline int HoughTransform::uniqueInCircle(const Matrix<T>& mtx, const HoughCircle& circ)
{
	int res = 0;
	int posX = circ.x;
	int posY = circ.y;
	int radius = circ.radius;
	std::set<T> unique;

	for(int y=posY-radius; y<=posY+radius; ++y)
	{
		for(int x=posX-radius; x<=posX+radius; ++x)
		{
			unique.insert(mtx(x,y));
		}
	}

	res = unique.size() - 1; 
	return abs(res);
	//return(unique.size() - 1);
}

template <typename T> inline void HoughTransform::histogramInCircle(const Matrix<T>& mtx, const HoughCircle& circ, Vector<T>& hist)
{
	uint x = circ.x;
	uint y = circ.y;
	int radius = circ.radius;
	int radSq = radius * radius;

	//hist.resize(256);
	hist.fill(0);

	for(int ry=-radius; ry<=radius; ++ry)
	{
		for(int rx=-radius; rx<=radius; ++rx)
		{
			if((rx*rx + ry*ry) <= radSq)
			{
				++hist[(int)mtx(x+rx, y+ry)];
			}
		}
	}
}

#endif
