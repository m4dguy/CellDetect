#include "OptionWindowGeneral.h"

OptionWindowGeneral::OptionWindowGeneral(void) : Fl_Window(_width, _height, "General Settings")
{
	this->color(FL_WHITE);
	this->begin();

	int h = -20;

	_checkEnforceResolution = new Fl_Check_Button(20, h+=30, _width, 20, "Enforce Default Resolution");
	_checkEnforceResolution->value(0);
	_checkEnforceResolution->tooltip("Enforces the default resolution of 512 x 512 pixels\nImages will be scaled accordingly");

	_checkNormalize = new Fl_Check_Button(20, h+=30, _width, 20, "normalize colours");
	_checkNormalize->value(1);
	_checkNormalize->tooltip("Lessens the effect of contrast and brightness enhancement\nDo not use if neither of both were changed");

	_checkImgDumps = new Fl_Check_Button(20, h+=30, _width, 20, "save processed images");
	_checkImgDumps->value(0);
	_checkImgDumps->tooltip("Saves the fully processed images");

	_checkFlagDumps = new Fl_Check_Button(20, h+=30, _width, 20, "save settings");
	_checkFlagDumps->value(1);
	_checkFlagDumps->tooltip("Saves all settings");

	//this->set_modal();

	//all done
	this->end();
	this->resize(this->x(), this->y(), this->w(), h+30);
}

OptionWindowGeneral::~OptionWindowGeneral(void)
{
	delete _checkImgDumps;
	delete _checkFlagDumps;
	delete _checkNormalize;
	delete _checkEnforceResolution;
}
