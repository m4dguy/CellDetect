#include "NuLLFile.h"

//hacky. fix this
template<typename T> int NuLLFile::writeCSV(const Matrix<T>& src, const char* filename)
{
	std::fstream stream;
	stream.open (filename, std::fstream::in | std::fstream::out);
	stream << src;
	stream.close();
	return stream.failbit;
}

template<typename T> int NuLLFile::writePGM(const Matrix<T>& src, const char* filename)
{
	size_t width = src.width();
	size_t height = src.height();
	FILE* outfile = fopen(filename, "wb");
	uchar pix;

	//header
	fprintf(outfile, "P5 \n");
	fprintf(outfile, "%ld %ld \n255\n",  width, height);
	
	//image
	for (uint y=0; y<height; y++)
	{
		for (uint x=0; x<width; x++)
		{
			pix = (uchar)(src(x,y));
			fwrite(&pix, sizeof(uchar), 1, outfile);
		}
	}

	return fclose(outfile);
}

//TODO: header
template<typename T> int NuLLFile::writePPM(const Matrix<T>& srcR, const Matrix<T>& srcG, const Matrix<T>& srcB, const char* filename)
{
	size_t width = srcR.width();
	size_t height = srcR.height();
	FILE* outfile = fopen(filename, "wb");
	uchar R, G, B;

	//header
	fprintf(outfile, "P6 \n");
	fprintf(outfile, "%ld %ld \n255\n",  width, height);
	
	//image
	for (uint y=0; y<height; y++)
	{
		for (uint x=0; x<width; x++)
		{
			R = (uchar)(srcR(x,y));
			G = (uchar)(srcG(x,y));
			B = (uchar)(srcB(x,y));
			fwrite (&R, sizeof(uchar), 1, outfile);
			fwrite (&G, sizeof(uchar), 1, outfile);
			fwrite (&B, sizeof(uchar), 1, outfile);
		}
	}
	
	return fclose(outfile);
}
