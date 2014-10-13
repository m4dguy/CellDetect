#ifndef	ACTIVECONTOURS_INL
#define ACTIVECONTOURS_INL

#include "ActiveContours.h"


template<typename T> void ActiveContours::updateStatistics(const Matrix<T>& lvlSet, const Matrix<T>& img, Statistics& stats)
{
	size_t width = lvlSet.width();
	size_t height = lvlSet.height();

	double avgIn = 0;
	double avgOut = 0; 

	uint area = 0;
	uint length = 0;

	for(uint y=0; y<height; ++y)
	{
		for(uint x=0; x<width; ++x)
		{
			if(lvlSet(x,y) >= 0)
			{
				avgIn += img(x,y);
				++area;

				if(lvlSet.getMirrored(x+1,y)<=0)
					++length;

				if(lvlSet.getMirrored(x-1,y)<=0)
					++length;

				if(lvlSet.getMirrored(x,y+1)<=0)
					++length;

				if(lvlSet.getMirrored(x,y-1)<=0)
					++length;
			}
			else
			{
				avgOut += img(x,y);
			}
		}
	}

	avgIn /= area;
	avgOut /= (width * height) - area;

	stats.averageIn = avgIn;
	stats.averageOut = avgOut;
	stats.area = area;
	stats.length = length;
}

template<typename T> void ActiveContours::updateProbabilities(const Matrix<T>& lvlSet, const Matrix<T>& img, Vector<double>& logProbsIn, Vector<double>& logProbsOut)
{
	size_t width = lvlSet.width();
	size_t height = lvlSet.height();

	double areaIn = 0;
	double areaOut = 0;

	logProbsIn.fill(1.);
	logProbsOut.fill(1.);

	for(uint y=0; y<height; ++y)
	{
		for(uint x=0; x<width; ++x)
		{
			if(lvlSet(x,y) >= 0)
			{
				++logProbsIn[(int)img(x,y)];
				++areaIn;
			}
			else
			{
				++logProbsOut[(int)img(x,y)];
			}
		}
	}
	
	areaOut = (width * height) - areaIn;
	for(uint i=0; i<logProbsIn.size(); ++i)
	{
		logProbsIn[i] = log(logProbsIn[i] / areaIn);
		logProbsOut[i] = log(logProbsOut[i] / areaOut);
	}
}

template<typename T> void ActiveContours::spf(const Matrix<T>& lvlSet, const Matrix<T>& img, Matrix<T>& dst)
{
	size_t width = lvlSet.width();
	size_t height = lvlSet.height();

	T val, maxDiv = 0;
	double avgIn = 0;
	double avgOut = 0;
	double avgInOut = 0;
	uint area = 0;
	
	for(uint y=0; y<height; ++y)
	{
		for(uint x=0; x<width; ++x)
		{
			if(lvlSet(x,y) >= 0)
			{
				avgIn += img(x,y);
				++area;
			}
			else
			{
				avgOut += img(x,y);
			}
		}
	}

	avgIn /= area;
	avgOut /= (width * height) - area;
	avgInOut = 0.5 * (avgIn + avgOut);

	for(uint y=0; y<height; ++y)
	{
		for(uint x=0; x<width; ++x)
		{
			dst(x,y) = val = img(x,y) - avgInOut;
			val = fabs(val);
			maxDiv = max(val, maxDiv);
		}
	}

	dst /= maxDiv;
}

template<typename T> void ActiveContours::initializeLevelSet(Matrix<T>& ini)
{
	size_t width = ini.width();
	size_t height = ini.height();

	Matrix<T> inside(width, height);
	Matrix<T> outside(width, height);

	//init by setting foreground shape
	initializeForeground(ini);
	
	NuLLProcessing::distanceTransform(ini, outside);
	NuLLProcessing::affineRescale(ini, ini, 255, 0);
	NuLLProcessing::distanceTransform(ini, inside);

	NuLLTools::mSqrt(inside, inside);
	NuLLTools::mSqrt(outside, outside);

	ini = inside - outside;
}

//
template<typename T> void ActiveContours::initializeForeground(Matrix<T>& ini, const int type)
{
	const size_t width = ini.width();
	const size_t height = ini.height();

	//init 
	switch(type)
	{
		case 0:
		{
			//circle init
			const int radius = (int)(width * .95 * .5);
			const uint cx = width/2;
			const uint cy = height/2;
			const double radSq = radius * radius;
			for(int ry=-radius; ry<radius; ++ry)
				for(int rx=-radius; rx<radius; ++rx)
					if((rx*rx+ry*ry) <= radSq)
						ini(cx+rx, cy+ry) = 255.;
			break;
		}

		case 1:
		{
			//small circles init
			const int rad = 8;
			int radSq = rad * rad;
			int dist = 20;
			for(uint y=dist; y<height; y+=dist)
				for(uint x=dist; x<width; x+=dist)
					for(int ry=-rad; ry<rad; ++ry)
						for(int rx=-rad; rx<rad; ++rx)
							if(rx*rx+ry*ry < radSq)
								ini(x+rx,y+ry) = 255;
			break;
		}
		
		case 2:
		{
			//big square init
			uint border = 100;
			for(uint y=border-1; y<height - border; ++y)
				for(uint x=border-1; x<width - border; ++x)
					ini(x,y) = 255;
			break;
		}
	}


	//evenly spaced pixel
	/*for(uint y=0; y<height; y+=2)
		for(uint x=0; x<width; x+=2)
			ini(x,y) = 255.;*/
}

template<typename T> void ActiveContours::initializeEdgeStoppingFunction(const Matrix<T>& src, Matrix<T>& edgeStop, const double epsilon)
{
	const size_t width = src.width();
	const size_t height = src.height();

	const T epsSq = epsilon * epsilon;
	T dx, dy;

	for(uint y=0; y<height; ++y)
	{
		for(uint x=0; x<width; ++x)
		{
			dx = (src.getMirrored(x+1,y) - src.getMirrored(x-1,y)) / 2.0;
			dy = (src.getMirrored(x,y+1) - src.getMirrored(x,y-1)) / 2.0;
			edgeStop(x,y) = 1.0 / sqrt(dx*dx + dy*dy + epsSq);
		}
	}
}

template<typename T> void ActiveContours::reinitialize(const Matrix<T>& src, Matrix<T>& dst)
{
	size_t width = src.width();
	size_t height = src.height();

	heaviside(src, dst);

	Matrix<T> inside(width, height);
	Matrix<T> outside(width, height);
	
	NuLLProcessing::distanceTransform(dst, outside);
	NuLLProcessing::affineRescale(dst, dst, 255, 0);
	NuLLProcessing::distanceTransform(dst, inside);

	NuLLTools::mSqrt(inside, inside);
	NuLLTools::mSqrt(outside, outside);

	dst = inside - outside;
}

template<typename T> void ActiveContours::heaviside(const Matrix<T>& src, Matrix<T>& dst)
{
	const size_t width = src.width();
	const size_t height = src.height();

	for(uint y=0; y<height; ++y)
		for(uint x=0; x<width; ++x)
			dst(x,y) = (src(x,y)>=0)? 1. : 0.;
}

template<typename T> void ActiveContours::regularisedHeaviside(const Matrix<T>& src, Matrix<T>& dst)
{
	const size_t width = src.width();
	const size_t height = src.height();

	for(uint y=0; y<height; ++y)
		for(uint x=0; x<width; ++x)
			dst(x,y) = .5*(1. + (2./PI) * atan(src(x,y) / EPSILON));
}

template<typename T> void ActiveContours::regionBased(const Matrix<T>& src, Matrix<T>& dst, const double mu, const double nu, const double lambda1, const double lambda2, const double tau, const double epsilon, const uint reini, uint iterations)
{
	const size_t width = src.width();
	const size_t height = src.height();

	const double epsSq = epsilon * epsilon;
	const double piInv = (1./PI);

	//initialization
	Matrix<T> cont(width, height);						//contour
	Matrix<T> tmp(width, height);						//for swapping

	initializeLevelSet(cont);

	Statistics stats;
	updateStatistics(cont, src, stats);

	bool stuck = 0;
	T dx, dy, dxx, dyy, dxy;
	T div, heavy, val;

	for(uint i=0; (i<iterations && !stuck); ++i)
	{
		stuck = 1;
		printf("iteration: %i\n", i);
		//derivatives for curvature motion
		for(uint y=0; y<height; ++y)
		{
			for(uint x=0; x<width; ++x)
			{
				if(x==0 || x==width-1)
				{
					if(x==0)
					{
						dx = cont(x+1,y) - cont(x,y);
						dxx = cont(x+2,y) - cont(x+1,y) + cont(x,y);
					}
					else
					{
						dx = cont(x,y) - cont(x-1,y);
						dxx = cont(x,y) - 2*cont(x-1,y) + cont(x-2,y);
					}
				}
				else
				{
					dx = (cont(x+1,y) - cont(x-1,y)) / 2.;
					dxx = cont(x+1,y) - 2.*cont(x,y) + cont(x-1,y);
				}
				
				if(y==0 || y==height-1)
				{
					if(y==0)
					{
						dy = cont(1,y+1) - cont(x,y);
						dyy = cont(x,y+2) - cont(x,y+1) + cont(x,y);
					}
					else
					{
						dy = cont(x,y) - cont(x,y-1);
						dyy = cont(x,y) - 2*cont(x,y-1) + cont(x,y-2);
					}
				}
				else
				{
					dy = (cont(x,y+1) - cont(x,y-1)) / 2.;
					dyy = cont(x,y+1) - 2.*cont(x,y) + cont(x,y-1);
				}

				if((x==0) || (y==0) || (x==width-1) || (y==height-1))
				{
					//corners
					if((x==0) && (y==0))
						dxy = cont(x+1,y+1) - cont(x,y+1) - (cont(x+1,y) - cont(x,y));

					if((x==0) && (y==height-1))
						dxy = cont(x+1,y) - cont(x,y) - (cont(x+1,y-1) - cont(x,y-1));

					if((x==width-1) && (y==0))
						dxy = cont(x,y+1) - cont(x-1,y+1) - (cont(x,y) - cont(x-1,y));

					if((x==width-1) && (y==height-1))
						dxy = cont(x,y) - cont(x-1,y) - (cont(x,y-1) - cont(x-1,y-1));

					//edges
					if((x==0) && (y!=0) && (y!=height-1))
						dxy = .5*((cont(x+1,y+1) - cont(x,y+1)) - (cont(x+1,y-1) - cont(x,y-1)));

					if((x==width-1) && (y!=0) && (y!=height-1))
						dxy = .5*((cont(x,y+1) - cont(x-1,y+1)) - (cont(x,y-1) - cont(x-1,y-1)));

					if((y==0) && (x!=0) && (x!=height-1))
						dxy = .5*((cont(x+1,y+1) - cont(x-1,y+1)) - (cont(x+1,y) - cont(x-1,y)));

					if((y==height-1) && (x!=0) && (x!=height-1))
						dxy = .5*((cont(x+1,y-1) - cont(x-1,y-1)) - (cont(x+1,y) - cont(x-1,y)));
				}
				else
				{
					dxy = .25*((cont(x+1,y+1) - cont(x+1,y-1)) - (cont(x-1,y+1) - cont(x-1,y-1)));
				}
				
				val = (dx*dx+dy*dy);
				val = sqrt(val*val*val);

				if(val)
					div = (dy*dy*dxx-2*dx*dy*dxy+dx*dx*dyy)/(val);
				else
					div = 0;

				//derivative of regularised heaviside
				heavy = piInv * (epsilon / (epsSq + cont(x,y)*cont(x,y)));

				tmp(x,y) = cont(x,y);
				tmp(x,y) += heavy * tau * (mu * div - nu - lambda1 * (src(x,y)-stats.averageIn) * (src(x,y)-stats.averageIn) + lambda2 * (src(x,y)-stats.averageOut) * (src(x,y)-stats.averageOut));

				stuck &= (tmp(x,y) <= 0);
			}
		}

		if(!(i%reini))
			reinitialize(tmp, cont);
		else
			cont.swap(tmp);

		updateStatistics(cont, src, stats);
	}

	heaviside(cont, dst);
}

template<typename T> void ActiveContours::edgeBased(const Matrix<T>& src, Matrix<T>& dst, const double tau, const double epsilon, const double alpha, const uint reini, const uint iterations)
{
	const size_t width = src.width();
	const size_t height = src.height();

	//initialization
	Matrix<T> cont(width, height);						//contour
	Matrix<T> tmp(width, height);						//for swapping
	Matrix<T> edgeStop(width, height);					//edge stopping term
	
	initializeLevelSet(cont);
	initializeEdgeStoppingFunction(src, edgeStop, epsilon);

	T gx, gy;
	T dx, dy, dxx, dyy, dxy;
	T div, grad, val;

	for(uint i=0; i<iterations; ++i)
	{
		//curvature calculation
		for(uint y=0; y<height; ++y)
		{
			for(uint x=0; x<width; ++x)
			{
				if(x==0 || x==width-1)
				{
					if(x==0)
					{
						dx = cont(x+1,y) - cont(x,y);
						dxx = cont(x+2,y) - cont(x+1,y) + cont(x,y);
					}
					else
					{
						dx = cont(x,y) - cont(x-1,y);
						dxx = cont(x,y) - 2*cont(x-1,y) + cont(x-2,y);
					}
				}
				else
				{
					dx = (cont(x+1,y) - cont(x-1,y)) / 2.;
					dxx = cont(x+1,y) - 2.*cont(x,y) + cont(x-1,y);
				}
				
				if(y==0 || y==height-1)
				{
					if(y==0)
					{
						dy = cont(1,y+1) - cont(x,y);
						dyy = cont(x,y+2) - cont(x,y+1) + cont(x,y);
					}
					else
					{
						dy = cont(x,y) - cont(x,y-1);
						dyy = cont(x,y) - 2*cont(x,y-1) + cont(x,y-2);
					}
				}
				else
				{
					dy = (cont(x,y+1) - cont(x,y-1)) / 2.;
					dyy = cont(x,y+1) - 2.*cont(x,y) + cont(x,y-1);
				}

				if((x==0) || (y==0) || (x==width-1) || (y==height-1))
				{
					//corners
					if((x==0) && (y==0))
						dxy = cont(x+1,y+1) - cont(x,y+1) - (cont(x+1,y) - cont(x,y));

					if((x==0) && (y==height-1))
						dxy = cont(x+1,y) - cont(x,y) - (cont(x+1,y-1) - cont(x,y-1));

					if((x==width-1) && (y==0))
						dxy = cont(x,y+1) - cont(x-1,y+1) - (cont(x,y) - cont(x-1,y));

					if((x==width-1) && (y==height-1))
						dxy = cont(x,y) - cont(x-1,y) - (cont(x,y-1) - cont(x-1,y-1));

					//edges
					if((x==0) && (y!=0) && (y!=height-1))
						dxy = .5*((cont(x+1,y+1) - cont(x,y+1)) - (cont(x+1,y-1) - cont(x,y-1)));

					if((x==width-1) && (y!=0) && (y!=height-1))
						dxy = .5*((cont(x,y+1) - cont(x-1,y+1)) - (cont(x,y-1) - cont(x-1,y-1)));

					if((y==0) && (x!=0) && (x!=height-1))
						dxy = .5*((cont(x+1,y+1) - cont(x-1,y+1)) - (cont(x+1,y) - cont(x-1,y)));

					if((y==height-1) && (x!=0) && (x!=height-1))
						dxy = .5*((cont(x+1,y-1) - cont(x-1,y-1)) - (cont(x+1,y) - cont(x-1,y)));
				}
				else
				{
					dxy = .25*((cont(x+1,y+1) - cont(x+1,y-1)) - (cont(x-1,y+1) - cont(x-1,y-1)));
				}
				
				grad = sqrt(dx*dx+dy*dy);
				
				val = dx*dx+dy*dy;
				val = val*val*val;

				if(grad)
					div = (dy*dy*dxx-2*dx*dy*dxy+dx*dx*dyy)/sqrt(val);
				else
					div = 0;

				gx = (edgeStop.getMirrored(x+1,y)-edgeStop.getMirrored(x-1,y))*.5;
				gy = (edgeStop.getMirrored(x,y+1)-edgeStop.getMirrored(x,y-1))*.5;

				val = gx * dx + gy * dy;
				
				tmp(x,y) = cont(x,y);
				tmp(x,y) += tau * (alpha * edgeStop(x,y) * div * grad + val);

			}
		}

		if(!(i%reini))
			reinitialize(tmp, cont);
		else
			cont.swap(tmp);
	}

	heaviside(cont, dst);
}


template<typename T> void ActiveContours::pdfBased(const Matrix<T> & src, Matrix<T>& dst, const double nu, const double epsilon, const double tau, const uint reini, const uint iterations)
{
	const size_t width = src.width();
	const size_t height = src.height();

	const double epsSq = epsilon * epsilon;
	const double piInv = (1./PI);

	//initialization
	Matrix<T> cont(width, height);						//contour
	Matrix<T> tmp(width, height);						//for swapping
	initializeLevelSet(cont);

	Vector<double> logProbsIn(256);
	Vector<double> logProbsOut(256);	
	updateProbabilities(cont, src, logProbsIn, logProbsOut);

	T dx, dy, dxx, dyy, dxy;
	T div, heavi, val;

	for(uint i=0; i<iterations; ++i)
	{
		//curvature calculation
		for(uint y=0; y<height; ++y)
		{
			for(uint x=0; x<width; ++x)
			{
				if(x==0 || x==width-1)
				{
					if(x==0)
					{
						dx = cont(x+1,y) - cont(x,y);
						dxx = cont(x+2,y) - cont(x+1,y) + cont(x,y);
					}
					else
					{
						dx = cont(x,y) - cont(x-1,y);
						dxx = cont(x,y) - 2*cont(x-1,y) + cont(x-2,y);
					}
				}
				else
				{
					dx = (cont(x+1,y) - cont(x-1,y)) / 2.;
					dxx = cont(x+1,y) - 2.*cont(x,y) + cont(x-1,y);
				}
				
				if(y==0 || y==height-1)
				{
					if(y==0)
					{
						dy = cont(1,y+1) - cont(x,y);
						dyy = cont(x,y+2) - cont(x,y+1) + cont(x,y);
					}
					else
					{
						dy = cont(x,y) - cont(x,y-1);
						dyy = cont(x,y) - 2*cont(x,y-1) + cont(x,y-2);
					}
				}
				else
				{
					dy = (cont(x,y+1) - cont(x,y-1)) / 2.;
					dyy = cont(x,y+1) - 2.*cont(x,y) + cont(x,y-1);
				}

				if((x==0) || (y==0) || (x==width-1) || (y==height-1))
				{
					//corners
					if((x==0) && (y==0))
						dxy = cont(x+1,y+1) - cont(x,y+1) - (cont(x+1,y) - cont(x,y));

					if((x==0) && (y==height-1))
						dxy = cont(x+1,y) - cont(x,y) - (cont(x+1,y-1) - cont(x,y-1));

					if((x==width-1) && (y==0))
						dxy = cont(x,y+1) - cont(x-1,y+1) - (cont(x,y) - cont(x-1,y));

					if((x==width-1) && (y==height-1))
						dxy = cont(x,y) - cont(x-1,y) - (cont(x,y-1) - cont(x-1,y-1));

					//edges
					if((x==0) && (y!=0) && (y!=height-1))
						dxy = .5*((cont(x+1,y+1) - cont(x,y+1)) - (cont(x+1,y-1) - cont(x,y-1)));

					if((x==width-1) && (y!=0) && (y!=height-1))
						dxy = .5*((cont(x,y+1) - cont(x-1,y+1)) - (cont(x,y-1) - cont(x-1,y-1)));

					if((y==0) && (x!=0) && (x!=height-1))
						dxy = .5*((cont(x+1,y+1) - cont(x-1,y+1)) - (cont(x+1,y) - cont(x-1,y)));

					if((y==height-1) && (x!=0) && (x!=height-1))
						dxy = .5*((cont(x+1,y-1) - cont(x-1,y-1)) - (cont(x+1,y) - cont(x-1,y)));
				}
				else
				{
					dxy = .25*((cont(x+1,y+1) - cont(x+1,y-1)) - (cont(x-1,y+1) - cont(x-1,y-1)));
				}
				
				val = dx*dx+dy*dy;
				val = val*val*val;

				if(val)
					div = (dy*dy*dxx-2*dx*dy*dxy+dx*dx*dyy)/sqrt(val);
				else
					div = 0;
				
				//derivative of regularised heaviside
				heavi = piInv * (epsilon / (epsSq + cont(x,y)*cont(x,y)));

				tmp(x,y) = cont(x,y);
				tmp(x,y) += heavi * tau * (nu * div + (logProbsOut[(int)src(x,y)] - logProbsIn[(int)(src(x,y))]));
			}
		}

		updateProbabilities(tmp, src, logProbsIn, logProbsOut);

		if(!(i%reini))
			reinitialize(tmp, cont);
		else
			cont.swap(tmp);
	}

	heaviside(cont, dst);
}


template<typename T> void ActiveContours::spfBased(const Matrix<T>& src, Matrix<T>& dst, const double alpha, const double tau, const uint iterations)
{
	const size_t width = src.width();
	const size_t height = src.height();

	//initialization
	Matrix<T> cont(width, height);						//contour
	Matrix<T> tmp(width, height);						//for swapping
	Matrix<T> spfVal(width, height);

	initializeForeground(cont, 1);
	for(uint y=0; y<height; ++y)
	{
		for(uint x=0; x<width; ++x)
		{
			if(cont(x,y)>0)
				cont(x,y) = 1;
			else
				cont(x,y) = -1;
		}
	}
	spf(cont, src, spfVal);

	T dx, dy;
	T grad;
	
	for(uint i=0; i<iterations; ++i)
	{
		for(uint y=0; y<height; ++y)
		{
			for(uint x=0; x<width; ++x)
			{
				dx = .5 * (cont.getMirrored(x+1,y) - cont.getMirrored(x-1,y));
				dy = .5 * (cont.getMirrored(x,y+1) - cont.getMirrored(x,y-1));

				grad = sqrt(dx*dx + dy*dy);

				tmp(x,y) = cont(x,y);
				tmp(x,y) += tau * (spfVal(x,y) * alpha * grad);

				if(cont(x,y)>0)
					cont(x,y) = 1;
				else
					cont(x,y) = -1;
			}
		}
		spf(cont, src, spfVal);
		cont.swap(tmp);
	}

	heaviside(cont, dst);
}

#endif