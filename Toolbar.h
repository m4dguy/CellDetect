#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <FL/Fl.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pack.H>

#include "Pixmaps.h"
#include "Utils.h"

/*
 * toolbar used in the mainframe
 *
 */

class Toolbar : public Fl_Group
{
public:
	Toolbar(Fl_Widget* owner=0, int posX=0, int posY=0, int width=512, int height=25);
	~Toolbar(void);

	void addWidget(Fl_Widget* w);			//adds widget in suitable position (left side)
	void addWidgetRight(Fl_Widget* w);		//adds widget at right side

	Fl_Button* buOpen;
	Fl_Button* buRun;
	Fl_Button* buSettings;
	Fl_Button* buClearJobs;

	Fl_Pixmap* _pixOpen;
	Fl_Pixmap* _pixRun;
	Fl_Pixmap* _pixSettings;
	Fl_Pixmap* _pixClear;

protected:
	int _posX;
	int _posXRight;

	Fl_Widget* _owner;
};

//callbacks




#endif //TOOLBAR_H
