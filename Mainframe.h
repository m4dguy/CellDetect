#ifndef MAINFRAME_H
#define MAINFRAME_H

//display images
#include <FL/Fl.H>
#include <FL/Fl_Shared_Image.H>

//required widgets
#include <FL/fl_ask.H>
#include <FL/Fl_Double_Window.h>
#include <FL/Fl_Button.h>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Progress.H>

#include <FL/fl_ask.H>

//custom widgets
#include "Toolbar.h"
#include "OptionWindowBright.h"
#include "OptionWindowFluo.h"
#include "OptionWindowGeneral.h"
#include "JobCreator.h"

//utils
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdlib.h>

//windows
#include <io.h>

//helpers
#include "Utils.h"
#include "natsort.h"

//main module
#include "CellDetector.h"

/*
 * main frame for user interaction
 * no mindblowing logic involved
 * loads images, builds jobs and calls CellDetector module
 *
 */

#define VERSION "0.5"
#define NAME "Auto Controlled Detection of Cells"
#define DEFAULT "resource/default.bmp"
#define ABOUT "resource/about.info"
#define MANUAL "resource/manual.info"


typedef struct
{
	std::string brightfield;
	std::string fluorescence;
	std::string inclusion;
}Job;


class Mainframe : public Fl_Double_Window
{
public:
	Mainframe(void);
	~Mainframe(void);

	int checkJobSanity(const std::vector<int> jobStruct) const;
	void makeJobs();
	void addFile(const char* file);
	void clearJobs();

	void runJob(uint f=0);
	void runAllJobs();

	void loadImage(const char* file);
	void sendFlags();

protected:
	static const uint _width = 512;
	static const uint _height = 512;

	std::vector<const char*> _files;
	std::vector<Job> _jobs;
	const char* _file;

	Fl_Shared_Image* _img;

	Toolbar* _tbar;
	OptionWindowBright* _opWinBright;
	OptionWindowFluo* _opWinFluo;
	OptionWindowGeneral* _opWinGeneral;
	JobCreator* _creator;
	Fl_Native_File_Chooser* _chooser;

	Flags _flags;
	CellDetector _detector;

	Fl_Menu_Bar* _menubar;
	Fl_Box* _boxImg;					//displays image
	Fl_Progress* _prog;

	std::string _manualText;
	std::string _aboutText;


	//callbacks
	//menubar
	friend static void cbOpen(Fl_Widget* o, void* w);
	friend static void cbExit(Fl_Widget* o, void* w);
	friend static void cbSettingsAll(Fl_Widget* o, void* w);
	friend static void cbSettingsBright(Fl_Widget* o, void* w);
	friend static void cbSettingsFluo(Fl_Widget* o, void* w);
	friend static void cbSettingsGeneral(Fl_Widget* o, void* w);
	friend static void cbSettingsCreator(Fl_Widget* o, void* w);
	friend static void cbModeFluo(Fl_Widget* o, void* w);
	friend static void cbManual(Fl_Widget* o, void* w);
	friend static void cbAbout(Fl_Widget* o, void* w);

	//buttons
	friend static void cbRun(Fl_Widget* o, void* w);
	friend static void cbClearJobs(Fl_Widget* o, void* w);
};

#endif //MAINFRAME_H
