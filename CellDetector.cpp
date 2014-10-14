#ifndef CELLDETECTOR_CPP
#define CELLDETECTOR_CPP

#include "CellDetector.h"

CellDetector::CellDetector() :_src(512), _preprocessed(512), _inclusion(512), _tmp(512), _res(512)
{

}

//convert image to matrix
void CellDetector::loadImage(const Fl_Image* src)
{
	_src.resize(src->w(), src->h());
	_preprocessed.resize(src->w(), src->h());
	_inclusion.resize(src->w(), src->h());
	_tmp.resize(src->w(), src->h());
	_res.resize(src->w(), src->h());
	FLTKInterface::imageToMatrix(*src, _src);
}

//set flags
void CellDetector::setFlags(const Flags& flags)
{
	_flags = flags;
	_flags.time = clock();
}

//run all methods for image analysis by hough transform
void CellDetector::runBrightfieldProtocoll()
{
	preprocessing();
	findCircles();
}

//assign fluorescence signals to cells
void CellDetector::runFluoProtocoll()
{
	measureFluoIntensity();
}

//run method for finding inclusion bodies
void CellDetector::runInclusionProtocoll()
{
	findInclusionBodies();
	assignInclusionBodies();
}

//preprocessing protocoll for brightfield images
void CellDetector::preprocessing()
{
	NuLLProcessing::differenceOfGaussians(_src, _tmp);
	NuLLProcessing::affineRescale(_tmp, _tmp);
	NuLLProcessing::medianFilter(_tmp, _preprocessed, 2);
	NuLLProcessing::firstDerivative(_preprocessed, _tmp);
	NuLLProcessing::automatedThresholding(_tmp, _preprocessed);
}

//main procedure for finding circles
//uses hough transform
void CellDetector::findCircles()
{
	//hough transform
	std::vector<HoughTransform::HoughCircle> circles;
	HoughTransform::houghCircles(_preprocessed, circles, _flags.circleThreshold, _flags.circleMinRad, _flags.circleMaxRad);
	HoughTransform::mergeCircles(circles, (uint)_flags.circleMaxRad);

	//classification
	//dumb classification for 2 classes only
	//1 = discocyte
	//2 = echinocyte
	double var;
	for(uint i=0; i<circles.size(); ++i)
	{
		var = HoughTransform::varianceInCircle(_preprocessed, circles[i], 3);
		circles[i].marker = (var < _flags.classificationThreshold)? 1 : 2;
	}
	
	if(_flags.imgOutput)
	{
		std::string target(currentReport().imgBrightfield);
		target.append(".ppm");
		_res.fill(0);
		HoughTransform::plotCircles(circles, _res);
		NuLLFile::writePPM(_res, _src, _res, target.c_str());
	}

	//circles to cells
	std::vector<Cell> foundCells;
	Cell newCell;
	for(uint i=0; i<circles.size(); ++i)
	{
		newCell.circle = circles[i];
		foundCells.push_back(newCell);
	}

	Report& rep = currentReport();
	rep.cells = foundCells;
}

//
void CellDetector::measureFluoIntensity()
{
	dType absInt;
	Report& rep = currentReport();
	std::vector<Cell>& cells = rep.cells;

	//normalize
	if(_flags.normalize)
		NuLLProcessing::affineRescale(_src, _src);

	//eliminate noise
	//NuLLProcessing::medianFilter(_src, 

	//determine absolute intensities
	for(uint i=0; i<cells.size(); ++i)
	{
		absInt = HoughTransform::averageInCircle(_src, cells[i].circle);
		cells[i].absoluteIntensity = absInt;
	}

	//expand circles and build foreground-background mask
	std::vector<HoughTransform::HoughCircle> circs;
	for(uint i=0; i<cells.size(); ++i)
		circs.push_back(cells[i].circle);

	_tmp.fill(0.);
	HoughTransform::plotCirclesFilled(circs, _tmp);

	//determine relative intensities
	for(uint i=0; i<cells.size(); ++i)
	{
		cells[i].localBackgroundIntensity = HoughTransform::averageInNeighbourhood(_src, _tmp, cells[i].circle, 7);
		cells[i].relativeIntensity = cells[i].absoluteIntensity / cells[i].localBackgroundIntensity;
		cells[i].intensityDifference = cells[i].absoluteIntensity - cells[i].localBackgroundIntensity;
		cells[i].activated = (cells[i].relativeIntensity > _flags.activationThreshold);
	}
}

void CellDetector::findInclusionBodies()
{
	if(!_flags.modeFluo)
	{
		dType mean = NuLLTools::mean(_src);
		dType var = NuLLTools::variance(_src);
		dType t = mean + var * _flags.inclusionBodyQuantile;

		NuLLProcessing::medianFilter(_src, _tmp);
		NuLLProcessing::thresholding(_tmp, _inclusion, t);
		NuLLProcessing::closing(_inclusion, _tmp, 3);
	}
	else
	{
		NuLLProcessing::medianFilter(_src, _tmp);
		NuLLProcessing::gaussianBlur(_tmp, _inclusion);

		ActiveContours::regionBased(_inclusion, _tmp, _flags.length, _flags.area, _flags.lambda1Fluo, _flags.lambda2Fluo, _flags.timeStepFluo, .1, _flags.reiniFluo, _flags.iterFluo);
		
		NuLLProcessing::affineRescale(_tmp, _inclusion);
		NuLLProcessing::closing(_inclusion, _tmp, 3);
	}

	if(_flags.imgOutput)
	{
		std::string target(currentReport().imgInclusion);
		target.append(".ppm");
		NuLLFile::writePPM(_tmp, _src, _tmp, target.c_str());
	}
}

void CellDetector::assignInclusionBodies()
{
	uint area, maxArea;
	uint bestCell;
	double areaPercentage;
	Report& rep = currentReport();
	std::vector<Cell>& cells = rep.cells;
	uint regions;

	regions = Regions::markRegions(_tmp, _inclusion);
	
	for(uint i=0; i<cells.size(); ++i)
		cells[i].inclusionBodies = 0;

	//assign each inclusion body to the cell which contains the majority of its pixels
	//ignore inclusion body candidates if their area relative to cell size is too big
	for(uint r=1; r<=regions; ++r)
	{
		maxArea = 0;
		bestCell = 0;
		areaPercentage = 0.;
		
		for(uint i=0; i<cells.size(); ++i)
		{
			area = HoughTransform::pixelValueInCircle(_inclusion, cells[i].circle, r);

			if(area >= maxArea)
			{
				maxArea = area;
				bestCell = i;
			}
		}

		if(!maxArea)
			continue;

		areaPercentage = (double)maxArea / (double)cells[bestCell].circle.area;

		if(areaPercentage < _flags.inclusionBodyPercentage)
			++cells[bestCell].inclusionBodies;
		
	}


	/*for(uint i=0; i<cells.size(); ++i)
	{
		covered = HoughTransform::foregroundPixelsInCircle(_inclusion, cells[i].circle);
		covered /= cells[i].circle.area;
		
		if(covered <= _flags.inclusionBodyPercentage)
			cells[i].inclusionBodies = HoughTransform::uniqueInCircle(_inclusion, cells[i].circle);
		else
			cells[i].inclusionBodies = 0;
	}*/
}

void CellDetector::createReport(const char* imgBright, const char* imgFluo, const char* imgInclusion)
{
	Report rep;
	rep.imgBrightfield = imgBright;
	rep.imgFluo = imgFluo;
	rep.imgInclusion = imgInclusion;
	_reports.push_back(rep);
}

Report& CellDetector::currentReport()
{
	return(_reports[_reports.size()-1]);
}

void CellDetector::flushReports()
{
	_reports.clear();
}

int CellDetector::dumpSingleImageReport()
{
	Report rep = currentReport();

	//building new filename
	const char* filename = "_results.csv";
	std::string dst(rep.imgBrightfield);
	dst.append(filename);

	FILE* outfile = fopen(dst.c_str(), "wb");

	//debug: if file can not be created
	if(!outfile)
		return 1;

	//header
	fprintf(outfile, "\"ID\"\t");
	fprintf(outfile, "\"x\"\t");
	fprintf(outfile, "\"y\"\t");
	fprintf(outfile, "\"radius\"\t");
	fprintf(outfile, "\"area\"\t");
	fprintf(outfile, "\"hough score\"\t");
	fprintf(outfile, "\"abs.intensity\"\t");
	fprintf(outfile, "\"abs.background intensity\"\t");
	fprintf(outfile, "\"intensity difference\"\t");
	fprintf(outfile, "\"rel.intensity\"\t");
	fprintf(outfile, "\"class\"\t");
	fprintf(outfile, "\"inclusion bodies\"\t");
	fprintf(outfile, "\"activated\"");

	//data
	std::vector<Cell> cells = rep.cells;
	char* celltype = "";
	char* activated = "";
	for(uint i=0; i<cells.size(); ++i)
	{
		if(cells[i].circle.marker == 1)
			celltype = "discocyte";
		else
			celltype = "echinocyte";

		if(cells[i].activated)
			activated = "yes";
		else
			activated = "no";

		fprintf(outfile, "\n%i\t%i\t%i\t%i\t%i\t%f\t%f\t%f\t%f\t%f\t%s\t%i\t%s",
			i, cells[i].circle.x, cells[i].circle.y, cells[i].circle.radius, cells[i].circle.area,
			cells[i].circle.score, cells[i].absoluteIntensity, cells[i].localBackgroundIntensity,
			cells[i].intensityDifference, cells[i].relativeIntensity,
			celltype, cells[i].inclusionBodies, activated);
	}

	return fclose(outfile);
}

int CellDetector::dumpCompleteReport()
{
	if(_flags.flagOutput)
		dumpFlags();

	std::string brightFile;
	std::string fluoFile;
	std::string inclusionFile;

	//building new filename
	const char* filename = "report.csv";
	std::string dst = getPath(currentReport().imgBrightfield);
	dst.append(filename);

	FILE* outfile = fopen(dst.c_str(), "wb");
	
	//debug: if file can not be created
	if(!outfile)
		return 1;

	//header
	fprintf(outfile, "\"ID\"\t");
	fprintf(outfile, "\"radius\"\t");
	fprintf(outfile, "\"area\"\t");
	fprintf(outfile, "\"hough score\"\t");
	fprintf(outfile, "\"abs.intensity\"\t");
	fprintf(outfile, "\"abs.background intensity\"\t");
	fprintf(outfile, "\"intensity difference\"\t");
	fprintf(outfile, "\"rel.intensity\"\t");
	fprintf(outfile, "\"class\"\t");
	fprintf(outfile, "\"inclusion bodies\"\t");
	fprintf(outfile, "\"activated\"\t");
	fprintf(outfile, "\"file(brightfield)\"\t");
	fprintf(outfile, "\"file(fluo)\"\t");
	fprintf(outfile, "\"file(inclusion bodies)\"");

	//data
	uint i=0;									//cell id
	Report rep;
	std::vector<Cell> cells;
	for(uint r=0; r<_reports.size(); ++r)
	{
		rep = _reports[r];
		brightFile = getFilename(rep.imgBrightfield);
		fluoFile = getFilename(rep.imgFluo);
		inclusionFile = getFilename(rep.imgInclusion);
		cells = rep.cells;
		char* celltype = "";
		char* activated = "";

		for(uint c=0; c<cells.size(); ++c)
		{
			if(cells[c].circle.marker == 1)
				celltype = "discocyte";
			else
				celltype = "echinocyte";

			if(cells[c].activated)
				activated = "yes";
			else
				activated = "no";

			fprintf(outfile, "\n%i\t%i\t%i\t%f\t%f\t%f\t%f\t%f\t%s\t%i\t%s\t%s\t%s\t%s",
				i, cells[c].circle.radius, cells[c].circle.area, cells[c].circle.score,
				cells[c].absoluteIntensity, cells[c].localBackgroundIntensity,
				cells[c].intensityDifference, cells[c].relativeIntensity, celltype,
				cells[c].inclusionBodies, activated, brightFile.c_str(), fluoFile.c_str(), inclusionFile.c_str());

			++i;
		}
	}

	return fclose(outfile);
}

int CellDetector::dumpFlags()
{
	_flags.time = clock() - _flags.time;

	//building new filename
	const char* filename = "flags.txt";
	std::string dst = getPath(currentReport().imgBrightfield);
	dst.append(filename);

	return _flags.dump(dst.c_str());
}

std::string CellDetector::getPath(const char* file)
{
	std::string res(file);
	unsigned int mark = res.rfind("\\") + 1;
	res.erase(res.begin()+mark, res.end());
	return res;
}

std::string CellDetector::getFilename(const char* file)
{
	std::string res(file);
	unsigned int mark = res.rfind("\\") + 1;
	res.erase(0, mark);
	return res;
}


#endif
