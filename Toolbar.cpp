#include "Toolbar.h"

Toolbar::Toolbar(Fl_Widget* owner, int posX, int posY, int width, int height) : Fl_Group(posX, posY, width, height)
{
	this->_posX = posX;
	this->_posXRight = width - 2;
	this->_owner = owner;

	this->type(Fl_Pack::HORIZONTAL);
	box(FL_UP_BOX);

	//build elements
	this->begin();
	
	buOpen = new Fl_Button(0, 0, height, height);
	_pixOpen = new Fl_Pixmap(Pixmaps::folder_xpm);
	buOpen->image(_pixOpen);
	this->addWidget(buOpen);

	buSettings = new Fl_Button(0, 0, height, height);
	_pixSettings = new Fl_Pixmap(Pixmaps::cog_xpm);
	buSettings->image(_pixSettings);
	this->addWidget(buSettings);

	buRun = new Fl_Button(0, 0, height, height);
	_pixRun = new Fl_Pixmap(Pixmaps::run_xpm);
	buRun->image(_pixRun);
	this->addWidget(buRun);

	buClearJobs = new Fl_Button(0, 0, height, height);
	_pixClear = new Fl_Pixmap(Pixmaps::cross_xpm);
	buClearJobs->image(_pixClear);
	this->addWidget(buClearJobs);

	//all done
	this->end();
}

Toolbar::~Toolbar(void)
{
	delete buOpen;
	delete _pixOpen;
	delete buRun;
	delete _pixRun;
	delete buSettings;
	delete _pixSettings;
}

void Toolbar::addWidget(Fl_Widget* w)
{
	this->add(w);
	w->resize(_posX, this->y(), w->w(), this->h());
	_posX += w->h();
}

void Toolbar::addWidgetRight(Fl_Widget* w)
{
	this->add(w);
	_posXRight -= w->w();
	w->resize(_posXRight, this->y(), w->w(), this->h());
}
