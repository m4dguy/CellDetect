#include "OptionWindowFluo.h"

OptionWindowFluo::OptionWindowFluo(void) : Fl_Window(_width, _height, "Fluorescence Protocoll")
{
	this->color(FL_WHITE);
	this->begin();

	mode = 0;
	int h = -20;

	_buMode = new Fl_Button(10, h+=30, _width-20, 20, "Toggle mode");

	//fast mode
	_inputQuantile = new Fl_Float_Input((_width/2)-10, h+=30, _width/2, 20, "Quantile: ");
	_inputQuantile->tooltip("Quantile for fast detection\nRefer to a quantile table for desired value");
	_inputQuantile->value("2.576");

	//accuracy mode
	_inputTimeStep = new Fl_Float_Input((_width/2)-10, h+=30, _width/2, 20, "Time Step: ");
	_inputTimeStep->tooltip("Size of timestep\nLarge values lead to numerical instability!");
	_inputTimeStep->value("0.1");
	_inputTimeStep->deactivate();

	_inputReini = new Fl_Int_Input((_width/2)-10, h+=30, _width/2, 20, "Reinitialization: ");
	_inputReini->tooltip("Frequency of reinitializations\nReinitializations require tine equal to one iteration");
	_inputReini->value("30");
	_inputReini->deactivate();

	_inputIter = new Fl_Int_Input((_width/2)-10, h+=30, _width/2, 20, "Iterations: ");
	_inputIter->tooltip("Maximal number of iterations");
	_inputIter->value("200");
	_inputIter->deactivate();

	_inputArea = new Fl_Int_Input((_width/2)-10, h+=30, _width/2, 20, "Area: ");
	_inputArea->tooltip("Importance of area size: ");
	_inputArea->value("400");
	_inputArea->deactivate();

	_inputLength = new Fl_Int_Input((_width/2)-10, h+=30, _width/2, 20, "Length: ");
	_inputLength->tooltip("Importance of region length: ");
	_inputLength->value("0");
	_inputLength->deactivate();

	_inputLambda1 = new Fl_Float_Input((_width/2)-10, h+=30, _width/2, 20, "Homogeneity (Foreground):");
	_inputLambda1->tooltip("Homogenity value for inclusion bodies");
	_inputLambda1->value("0.1");
	_inputLambda1->deactivate();

	_inputLambda2 = new Fl_Float_Input((_width/2)-10, h+=30, _width/2, 20, "Homogeneity: (Background)");
	_inputLambda2->tooltip("Homogenity value for background");
	_inputLambda2->value("1.0");
	_inputLambda2->deactivate();


	//general options
	_inputPercentageThres = new Fl_Float_Input((_width/2)-10, h+=30, _width/2, 20, "Inclusion body threshold: ");
	_inputPercentageThres->tooltip("Dismisses activated cells for inclusion body detection\nIf given percentage of pixels inside the cell are interpreted as inclusion bodies, they are ignored");
	_inputPercentageThres->value("0.5");

	_inputActivationThres = new Fl_Float_Input((_width/2)-10, h+=30, _width/2, 20, "Activation threshold: ");
	_inputActivationThres->tooltip("Marks cells as activated if their intensity is above given value");
	_inputActivationThres->value("1.1");

	//all done
	this->end();
	this->resize(this->x(), this->y(), this->w(), h+30);
}

OptionWindowFluo::~OptionWindowFluo(void)
{
	delete _buMode;

	delete _inputQuantile;

	delete _inputTimeStep;
	delete _inputReini;
	delete _inputIter;
	delete _inputLambda1;
	delete _inputLambda2;
}

void OptionWindowFluo::toggleMode()
{
	mode = !mode;
	if(mode)
	{
		_buMode->label("switch to fast mode");

		_inputTimeStep->activate();
		_inputReini->activate();
		_inputIter->activate();
		_inputArea->activate();
		_inputLength->activate();
		_inputLambda1->activate();
		_inputLambda2->activate();

		_inputQuantile->deactivate();
	}
	else
	{
		_buMode->label("switch to active contours");

		_inputTimeStep->deactivate();
		_inputReini->deactivate();
		_inputIter->deactivate();
		_inputArea->deactivate();
		_inputLength->deactivate();
		_inputLambda1->deactivate();
		_inputLambda2->deactivate();

		_inputQuantile->activate();
	}
}
