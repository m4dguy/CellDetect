#ifndef OPTIONWINDOWBRIGHT_H
#define OPTIONWINDOWBRIGHT_H

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

class OptionWindowBright : public Fl_Window
{
public:
	OptionWindowBright(void);
	~OptionWindowBright(void);
	
	//hough
	Fl_Int_Input* _inputMinRad;
	Fl_Int_Input* _inputMaxRad;
	Fl_Int_Input* _inputCircleThres;
	
	//classification
	Fl_Int_Input* _inputClassThres;

protected:
	bool mode;
	static const uint _width = 380;
	static const uint _height = 370;
};

#endif //OPTIONWINDOWBRIGHT_H
