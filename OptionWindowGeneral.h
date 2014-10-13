#ifndef OPTIONWINDOWGENERAL_H
#define OPTIONWINDOWGENERAL_H

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

class OptionWindowGeneral : public Fl_Window
{
public:
	OptionWindowGeneral(void);
	~OptionWindowGeneral(void);
	
	//general params	
	Fl_Check_Button* _checkNormalize;
	Fl_Check_Button* _checkEnforceResolution;

	Fl_Check_Button* _checkFlagDumps;
	Fl_Check_Button* _checkImgDumps;


protected:
	bool mode;
	static const uint _width = 380;
	static const uint _height = 370;
};

#endif //OPTIONWINDOWGENERAL_H
