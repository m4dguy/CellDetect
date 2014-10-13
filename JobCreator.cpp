#include "JobCreator.h"

JobCreator::JobCreator(void) : Fl_Window(_width, _height+_offset, "Job Creator")
{
	this->color(FL_WHITE);
	this->begin();

	_buAdd = new Fl_Button(0, 0, _width/2, 20, "add task");
	_buAdd->callback(cbAddChoice, (void*)this);
	
	_buRemove = new Fl_Button(_width/2, 0, _width/2, 20, "remove task");
	_buRemove->callback(cbRemoveChoice, (void*)this);

	_scrollTasks = new Fl_Scroll(0, _offset+20, _width, _height-_offset-20);

	addChoice();
	_tasks[0]->value(0);
	addChoice();
	_tasks[1]->value(3);
	addChoice();
	_tasks[2]->value(4);


	//all done
	this->end();
	//this->resizable(this);
	//this->resize(this->x(), this->y(), this->w(), this->h);
}

JobCreator::~JobCreator(void)
{
	for(uint i=0; i<_tasks.size(); ++i)
		delete _tasks[i];

	delete _buAdd;
	delete _buRemove;
}

void JobCreator::addChoice(void)
{
	//char label[12];
	int number = _tasks.size();
	//sprintf(label, "image %d: ", number);
	const char* label = "";

	Fl_Choice* newChoice = new Fl_Choice(20, 0, _width-40, 20, label);
	newChoice->add("brightfield");
	newChoice->add("fluorescence");
	newChoice->add("inclusion bodies");
	newChoice->add("fluorescence + inclusion bodies");
	newChoice->add("ignore");
	_tasks.push_back(newChoice);

	this->_scrollTasks->add(newChoice);
	newChoice->value(4);
	newChoice->position(20, _offset+20*(number+1));
	
	this->redraw();
}

void JobCreator::removeChoice(void)
{
	if(_tasks.size() <= 2)
	{
		fl_message("at least 2 tasks required required");
		return;
	}

	this->_scrollTasks->remove(_tasks[_tasks.size()-1]);
	delete _tasks[_tasks.size()-1];
	_tasks.pop_back();

	this->redraw();
}

std::vector<int> JobCreator::getJobStructure()
{
	std::vector<int> res;
	res.resize(_tasks.size());

	for(uint i=0; i<_tasks.size(); ++i)
		res[i] = _tasks[i]->value();

	return res;
}

void cbAddChoice(Fl_Widget* o, void* w)
{
	JobCreator* creator = (JobCreator*) w;
	creator->addChoice();
}

void cbRemoveChoice(Fl_Widget* o, void* w)
{
	JobCreator* creator = (JobCreator*) w;
	creator->removeChoice();
}
