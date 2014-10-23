#ifndef CELLDETECTOR_H
#define CELLDETECTOR_H

#include <vector>
#include <string>
#include <time.h>

#include "Matrix.h"
#include "Vector.h"

#include "FLTKInterface.inl"
#include "NuLLProcessing.inl"
#include "NuLLTools.inl"
#include "NuLLFile.inl"

#include "HoughTransform.inl"
#include "Regions.inl"
#include "ActiveContours.inl"


typedef double dType;					//change to double for high precision

//flags and parameters
typedef struct
{
	int modeFluo;							//0=fast, 1=active contours

	//hough transform
	double circleThreshold;					//flag
	int circleMinRad;						//flag
	int circleMaxRad;						//flag
	
	//classification
	double classificationThreshold;			//flag
	double activationThreshold;				//flag
	
	//inclusion bodies
	double inclusionBodyQuantile;			//flag
	double inclusionBodyPercentage;			//flag
	
	//inclusion bodies: active contours
	double timeStepFluo;					//flag
	int reiniFluo;							//flag
	int iterFluo;							//flag
	int area;								//flag
	int length;								//flag
	double lambda1Fluo;						//flag
	double lambda2Fluo;						//flag

	//additional
	int normalize;							//apply affine rescale

	//internal
	int imgOutput;							//set to allow image dumps
	int flagOutput;							//dump flags
	clock_t time;							//execution time

	int dump(const char* dst)
	{
		FILE* outfile = fopen(dst, "wb");
		fprintf(outfile, "circleThreshold:\t\t%f\n", circleThreshold);
		fprintf(outfile, "circleMinRad:\t\t%i\n", circleMinRad);
		fprintf(outfile, "circleMaxRad:\t\t%i\n", circleMaxRad);
		fprintf(outfile, "\n");
		fprintf(outfile, "classificationThreshold:\t\t%f\n", classificationThreshold);
		fprintf(outfile, "activation threshold:\t\t%f\n", activationThreshold);
		fprintf(outfile, "\n");

		if(modeFluo)
		{
			fprintf(outfile, "inclusion body detection:\t\tactive contours\n");
			fprintf(outfile, "time step:\t\t%f\n", timeStepFluo);
			fprintf(outfile, "reinitialization:\t\t%i\n", reiniFluo);
			fprintf(outfile, "iterations:\t\t%i\n", iterFluo);
			fprintf(outfile, "area:\t\t%i\n", area);
			fprintf(outfile, "length:\t\t%i\n", length);
			fprintf(outfile, "homogeneity (foreground):\t\t%f\n", lambda1Fluo);
			fprintf(outfile, "homogeneity (background):\t\t%f\n", lambda2Fluo);
		}
		else
		{
			fprintf(outfile, "inclusion body detection:\t\tthresholding\n");
			fprintf(outfile, "quantile:\t\t%f\n", inclusionBodyQuantile);
		}

		fprintf(outfile, "inclusion body percentage:\t\t%f\n", inclusionBodyPercentage);
		fprintf(outfile, "\n");
		fprintf(outfile, "imgOutput:\t\t%i\n", imgOutput);
		fprintf(outfile, "normalize:\t\t%i\n", normalize);
		fprintf(outfile, "\n");
		fprintf(outfile, "execution time:\t\t%f\n", (float)(time / CLOCKS_PER_SEC));

		return fclose(outfile);
	}

}Flags;

typedef struct
{
	HoughTransform::HoughCircle circle;				//position, radius, score
	double absoluteIntensity;						//absolute fluorescence intensity (average)
	double localBackgroundIntensity;				//absolule fluorescnece intensity of background (average)
	double relativeIntensity;						//fluorescence intensity relative to background
	double intensityDifference;						//difference of intensities

	int inclusionBodies;							//number of inclusion bodies
	bool activated;									//mark if cell glows strong enough
}Cell;

//results of single experiment (image pair)
typedef struct
{
	std::vector<Cell> cells;
	std::string imgBrightfield;
	std::string imgFluo;
	std::string imgInclusion;
}Report;


class CellDetector
{

public:
	CellDetector();
	~CellDetector(){};

	void loadImage(const Fl_Image* src);
	void setFlags(const Flags& flags);

	//brightfield protocoll
	void runBrightfieldProtocoll();
	void preprocessing();
	void findCircles();
	
	//fluorescence image protocoll
	void runFluoProtocoll();
	void measureFluoIntensity();

	void runInclusionProtocoll();
	void findInclusionBodies();
	void assignInclusionBodies();

	//saving and displaying results
	void createReport(std::string imgBright, std::string imgFluo, std::string imgInclusion);
	Report& currentReport();
	void flushReports();
	int dumpSingleImageReport();
	int dumpCompleteReport();
	int dumpStatistics();
	
	int dumpFlags();

	//util
	std::string getPath(std::string file);
	std::string getFilename(std::string file);

protected:
	std::string directory;
	Flags _flags;
	std::vector<Report> _reports;

	//main working matrices
	Matrix<dType> _src;
	Matrix<dType> _preprocessed;
	Matrix<dType> _inclusion;
	Matrix<dType> _res;

	//temporary matrix for operations
	Matrix<dType> _tmp;

};

#endif // CELLDETECTOR_H
