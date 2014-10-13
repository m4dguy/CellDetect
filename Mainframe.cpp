#include "Mainframe.h"

Mainframe::Mainframe(void) : Fl_Double_Window(_width, _height, NAME)
{
	Fl::visual(FL_RGB);

	//building the filechooser
	_chooser = new Fl_Native_File_Chooser();
	_chooser->title("Pick File(s)");
	_chooser->filter("Bitmap\t*.bmp\nJPG\t*.jpg, *.jpeg\nPNG\t*.png\nPortable Anymap\t*.pnm, *.pbm, *.pgm, *.ppm");
	_chooser->directory(".");
	_chooser->options(Fl_Native_File_Chooser::PREVIEW);
	_chooser->options(Fl_Native_File_Chooser::SAVEAS_CONFIRM);
	_chooser->options(Fl_Native_File_Chooser::NEW_FOLDER);

	_opWinBright = new OptionWindowBright;
	_opWinFluo = new OptionWindowFluo;
	_opWinGeneral = new OptionWindowGeneral;
	_creator = new JobCreator;

	this->begin();
	//build menu
	_menubar = new Fl_Menu_Bar(0, 0, this->w(), 25);
	_menubar->add("&File/&Open", 0, cbOpen, (void*)this);
	_menubar->add("&File/&Run", 0, cbRun, (void*)this);
	_menubar->add("&File/&Exit", 0, cbExit, (void*)this);

	_menubar->add("&Settings/&Brightfield Parameters", 0, cbSettingsBright, (void*)this);
	_menubar->add("&Settings/&Fluorescence Parameters", 0, cbSettingsFluo, (void*)this);
	_menubar->add("&Settings/&General Parameters", 0, cbSettingsGeneral, (void*) this);
	_menubar->add("&Settings/&Job Creator", 0, cbSettingsCreator, (void*)this);

	_menubar->add("&Help/&Manual", 0, cbManual, (void*)this);
	_menubar->add("&Help/&About", 0, cbAbout, (void*)this);

	//add toolbar
	_tbar = new Toolbar(this, 0, 26, _width);
	_tbar->buOpen->callback(cbOpen, (void*)this);
	_tbar->buRun->callback(cbRun, (void*)this);
	_tbar->buSettings->callback(cbSettingsAll, (void*)this);
	_tbar->buClearJobs->callback(cbClearJobs, (void*)this);
	_tbar->clear_visible_focus();
	
	_prog = new Fl_Progress(0, 0, 250, 10, "No jobs in queue");
	_prog->color(0x88888800);
	_prog->selection_color(0x00660000);
	_prog->labelcolor(FL_WHITE);
	_tbar->addWidgetRight(_prog);
	
	_opWinFluo->_buMode->callback(cbModeFluo, (void*)this);
	
	
	//build user info
	std::string line;
	std::ifstream fs;
	fs.open(MANUAL, std::fstream::in);
	while (std::getline(fs, line))
	{
		_manualText.append(line);
		_manualText.append("\n");
	}
	fs.close();

	std::ifstream fs2;
	fs2.open(ABOUT, std::fstream::in);
	while (std::getline(fs2, line))
	{
		_aboutText.append(line);
		_aboutText.append("\n");
	}
	fs2.close();
	
	
	//build interface
	_img = NULL;
	_file = DEFAULT;					//set to default; mainly for debug reasons
	_boxImg = new Fl_Box(0, (_menubar->h()+_tbar->h()+2), this->w(), this->h());
	loadImage(DEFAULT);
	
	
	//all done
	this->end();
	this->show();
}

Mainframe::~Mainframe(void)
{
}

int Mainframe::checkJobSanity(const std::vector<int> jobStruct) const
{
	for(uint i=0; i<jobStruct.size(); ++i)
		if(jobStruct[i] == 0)
			return 1;

	return false;
}

//constructs jobs before according to scheme in job creator
//required before running analysis pipeline
void Mainframe::makeJobs()
{
	this->cursor(FL_CURSOR_WAIT);

	// fix: files are not properly sorted under Win XP
	// sort by natural order to fix issue
	std::sort(_files.begin(), _files.end(), stringcmp);

	std::vector<int> jobStruct = _creator->getJobStructure();
	int taskCount = jobStruct.size();

	if(!checkJobSanity(jobStruct))
	{
		fl_message("Unable to build jobs.\nNo brightfield image in Job Creator defined.");
		this->cursor(FL_CURSOR_DEFAULT);
		_creator->show();
		Fl::check();
		return;
	}

	Job job;
	int task;

	job.brightfield = "";
	job.fluorescence = ""; 
	job.inclusion = "";

	for(uint i=0; i<_files.size(); ++i)
	{
		task = jobStruct[i%taskCount];
		switch(task)
		{
			case 0:
				job.brightfield = _files[i];
				break;
			case 1:
				job.fluorescence = _files[i];
				break;
			case 2:
				job.inclusion = _files[i];
				break;
			case 3:
				job.fluorescence = _files[i];
				job.inclusion = _files[i];
				break;
			default:
				break;
		}

		if((i%taskCount) == (taskCount-1))
		{
			_jobs.push_back(job);
			job.brightfield = "";
			job.fluorescence = ""; 
			job.inclusion = "";
		}
	}

	char label[18];
	sprintf(label, "%i jobs in queue", _jobs.size());
	_prog->label(label);
	_prog->value(0.);

	this->cursor(FL_CURSOR_DEFAULT);
	Fl::check();
}

//gather flags from GUI and pass to detector
void Mainframe::sendFlags()
{
	//brightfield
	_flags.circleMinRad = atoi(_opWinBright->_inputMinRad->value());
	_flags.circleMaxRad = atoi(_opWinBright->_inputMaxRad->value());
	_flags.circleThreshold = atof(_opWinBright->_inputCircleThres->value());
	_flags.classificationThreshold = atof(_opWinBright->_inputClassThres->value());

	_flags.activationThreshold = atof(_opWinFluo->_inputActivationThres->value());
	
	_flags.inclusionBodyQuantile = atof(_opWinFluo->_inputQuantile->value());
	_flags.inclusionBodyPercentage = atof(_opWinFluo->_inputPercentageThres->value());

	//inclusion bodies: mode
	_flags.modeFluo = _opWinFluo->mode;

	//inclusion bodies (active contours)
	_flags.timeStepFluo = atof(_opWinFluo->_inputTimeStep->value());
	_flags.reiniFluo = atoi(_opWinFluo->_inputReini->value());
	_flags.iterFluo = atoi(_opWinFluo->_inputIter->value());
	_flags.area = atoi(_opWinFluo->_inputArea->value());
	_flags.length = atoi(_opWinFluo->_inputLength->value());
	_flags.lambda1Fluo = atof(_opWinFluo->_inputLambda1->value());
	_flags.lambda2Fluo = atof(_opWinFluo->_inputLambda2->value());
	
	//inclusion bodies (quantile)
	_flags.inclusionBodyQuantile = atof(_opWinFluo->_inputQuantile->value());

	//general flags
	_flags.normalize = _opWinGeneral->_checkNormalize->value();
	_flags.flagOutput = _opWinGeneral->_checkFlagDumps->value();
	_flags.imgOutput = _opWinGeneral->_checkImgDumps->value();
	_flags.time = clock();

	_detector.setFlags(_flags);
}

void Mainframe::addFile(const char* file)
{
	_files.push_back(file);
}

void Mainframe::clearJobs()
{
	_jobs.clear();
	_files.clear();

	char label[18];
	sprintf(label, "No jobs in queue");
	_prog->label(label);
	
	Fl::check();
}

void Mainframe::runJob(uint f)
{
	Job j = _jobs[f];
	_detector.createReport(j.brightfield, j.fluorescence, j.inclusion);
	
	if(j.brightfield != "")
	{
		loadImage(j.brightfield);
		_detector.loadImage(_img);
		_detector.runBrightfieldProtocoll();
	}

	if(j.fluorescence != "")
	{
		loadImage(j.fluorescence);
		_detector.loadImage(_img);
		_detector.runFluoProtocoll();
	}

	if(j.inclusion != "")
	{
		loadImage(j.inclusion);
		_detector.loadImage(_img);
		_detector.runInclusionProtocoll();
	}

	_detector.dumpSingleImageReport();

	char label[5];
	sprintf(label, "%i%%", ((f+1)*100)/(_jobs.size()));
	_prog->label(label);
	_prog->value((float)f+1);
	Fl::check();
}

void Mainframe::runAllJobs()
{
	if(!_jobs.size())
	{
		fl_alert("No jobs in queue");
		return;
	}

	this->cursor(FL_CURSOR_WAIT);

	_prog->minimum(0.);
	_prog->maximum(float(_jobs.size()));
	_prog->label("0%");

	_detector.flushReports();
	sendFlags();
	for(uint i=0; i<_jobs.size(); ++i)
		runJob(i);

	_detector.dumpCompleteReport();
	clearJobs();

	_prog->label("done");
	Fl::check();
	this->cursor(FL_CURSOR_DEFAULT);
}

void Mainframe::loadImage(const char* file)
{
	_file = file;

	if(file == "")
		return;

	if(_img)
		_img->release();

	//_img = new Fl_BMP_Image(_file);
	_img = Fl_Shared_Image::get(file);
	this->size(_img->w(), (_img->h()+_menubar->h()+_tbar->h()+2));

	_boxImg->image(_img);
	_boxImg->size(_img->w(), _img->h());
	_boxImg->redraw();
	Fl::check();
}

void cbOpen(Fl_Widget* o, void* w)
{
	Mainframe* frame = (Mainframe*) w;
	Fl_Native_File_Chooser* chooser = frame->_chooser;
	chooser->type(Fl_Native_File_Chooser::BROWSE_MULTI_FILE);

	chooser->show();

	if(!chooser->count())
		return;

	for(int i=0; i<chooser->count(); ++i)
		frame->addFile(chooser->filename(i));

	frame->makeJobs();
	frame->loadImage(chooser->filename());
}

void cbExit(Fl_Widget* o, void* w)
{
	exit(0);
}

void cbSettingsAll(Fl_Widget* o, void* w)
{
	Mainframe* frame = (Mainframe*) w;
	
	frame->_opWinBright->position(frame->x()+frame->w()+15, frame->y());
	frame->_opWinFluo->position(frame->x()+frame->w()+15, frame->y()+frame->_opWinBright->h()+30);
	frame->_opWinGeneral->position(frame->x()+frame->w()+15, frame->y()+frame->_opWinBright->h()+30+frame->_opWinFluo->h()+30);

	if(frame->_opWinBright->shown() || frame->_opWinFluo->shown())
	{
		frame->_opWinBright->hide();
		frame->_opWinFluo->hide();
		frame->_opWinGeneral->hide();
		frame->_creator->hide();
	}
	else
	{
		frame->_opWinBright->show();
		frame->_opWinFluo->show();
		frame->_opWinGeneral->show();
	}
}

void cbSettingsBright(Fl_Widget* o, void* w)
{
	Mainframe* frame = (Mainframe*) w;
	frame->_opWinBright->position(frame->x()+frame->w()+15, frame->y());

	if(frame->_opWinBright->shown())
		frame->_opWinBright->hide();
	else
		frame->_opWinBright->show();
}

void cbSettingsFluo(Fl_Widget* o, void* w)
{
	Mainframe* frame = (Mainframe*) w;
	frame->_opWinFluo->position(frame->x()+frame->w()+15, frame->y());

	if(frame->_opWinFluo->shown())
		frame->_opWinFluo->hide();
	else
		frame->_opWinFluo->show();
}

void cbSettingsGeneral(Fl_Widget* o, void* w)
{
	Mainframe* frame = (Mainframe*) w;
	frame->_opWinGeneral->position(frame->x()+frame->w()+15, frame->y());

	if(frame->_opWinGeneral->shown())
		frame->_opWinGeneral->hide();
	else
		frame->_opWinGeneral->show();
}

void cbSettingsCreator(Fl_Widget* o, void* w)
{
	Mainframe* frame = (Mainframe*) w;
	frame->_creator->position(frame->x()+frame->w()+15, frame->y());

	if(frame->_creator->shown())
		frame->_creator->hide();
	else
		frame->_creator->show();
}

void cbModeFluo(Fl_Widget* o, void* w)
{
	Mainframe* frame = (Mainframe*) w;
	frame->_opWinFluo->toggleMode();
	Fl::check();
}

void cbManual(Fl_Widget* o, void* w)
{
	Mainframe* frame = (Mainframe*) w;
	fl_message(frame->_manualText.c_str());
}

void cbAbout(Fl_Widget* o, void* w)
{
	Mainframe* frame = (Mainframe*) w;
	fl_message(frame->_aboutText.c_str());
}

void cbRun(Fl_Widget* o, void* w)
{
	Mainframe* frame = (Mainframe*) w;
	frame->runAllJobs();
}

void cbClearJobs(Fl_Widget* o, void* w)
{
	Mainframe* frame = (Mainframe*) w;
	frame->clearJobs();
}
