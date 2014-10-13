#include "OptionWindowBright.h"

OptionWindowBright::OptionWindowBright(void) : Fl_Window(_width, _height, "Brightfield Protocoll")
{
	this->color(FL_WHITE);
	this->begin();

	int h = -20;

	//fast mode
	_inputMinRad = new Fl_Int_Input((_width/2)-10, h+=30, _width/2, 20, "minimal radius: ");
	_inputMinRad->tooltip("Minimal radius for cell detection");
	_inputMinRad->value("15");
	_inputMaxRad = new Fl_Int_Input((_width/2)-10, h+=30, _width/2, 20, "maximal radius: ");
	_inputMaxRad->tooltip("Maximal radius for cell detection");
	_inputMaxRad->value("30");
	
	_inputCircleThres = new Fl_Int_Input((_width/2)-10, h+=30, _width/2, 20, "circle threshold: ");
	_inputCircleThres->tooltip("Tolerance value for circle detection\nUse small values for increased sensitivity");
	_inputCircleThres->value("130");

	//optional
	_inputClassThres = new Fl_Int_Input((_width/2)-10, h+=30, _width/2, 20, "classification threshold: ");
	_inputClassThres->tooltip("Value for classification\nThe higher the value, the less likely a cell is classified as an echinocyte");
	_inputClassThres->value("10000");

	//this->set_modal();

	//all done
	this->end();
	this->resize(this->x(), this->y(), this->w(), h+30);
}

OptionWindowBright::~OptionWindowBright(void)
{
	delete _inputMinRad;
	delete _inputMaxRad;
	delete _inputCircleThres;

	delete _inputClassThres;
}
