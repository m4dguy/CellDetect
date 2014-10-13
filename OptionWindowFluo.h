#ifndef OPTIONWINDOWFLUO_H
#define OPTIONWINDOWFLUO_H

//display images
#include <FL/Fl.H>

//required widgets
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Check_Button.H>

#include "Utils.h"

/*
 * frame for algorithm parameters
 *
 */

 class OptionWindowFluo : public Fl_Window
{
public:
	OptionWindowFluo(void);
	~OptionWindowFluo(void);

	void toggleMode(void);

	bool mode;

	//toggle thresholding/ active contours
	Fl_Float_Input* _inputPercentageThres;		//neglect inclusion bodies if too big
	Fl_Float_Input* _inputActivationThres;		//mark cells as activated if bright enough
	Fl_Button* _buMode;							//switch between thresholding and active contours

	//fast mode
	Fl_Float_Input* _inputQuantile;				//set quantile for segmentation by thresholding

	//contours
	Fl_Float_Input* _inputTimeStep;
	Fl_Int_Input* _inputReini;
	Fl_Int_Input* _inputIter;
	Fl_Int_Input* _inputArea;
	Fl_Int_Input* _inputLength;
	Fl_Float_Input* _inputLambda1;
	Fl_Float_Input* _inputLambda2;

protected:
	static const uint _width = 380;
	static const uint _height = 190;
};

#endif //OPTIONWINDOWFLUO_H
