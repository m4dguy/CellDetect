#ifndef JOBCREATOR_H
#define JOBCREATOR_H

//display images
#include <FL/Fl.H>

//required widgets
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Scroll.H>
#include <FL/fl_ask.H>

#include <vector>
#include "Utils.h"

/*
 * frame for algorithm parameters
 *
 */

class JobCreator : public Fl_Window
{
public:
	JobCreator(void);
	~JobCreator(void);
	
	//general params
	Fl_Button* _buAdd;
	Fl_Button* _buRemove;

	Fl_Scroll* _scrollTasks;
	std::vector<Fl_Choice*> _tasks;

	void addChoice(void);
	void removeChoice(void);

	std::vector<int> getJobStructure();

protected:
	static const uint _offset = 20;

	static const uint _width = 200;
	static const uint _height = 180;

	//event callbacks
	friend static void cbAddChoice(Fl_Widget* o, void* w);
	friend static void cbRemoveChoice(Fl_Widget* o, void* w);
};

#endif //JOBCREATOR_H
