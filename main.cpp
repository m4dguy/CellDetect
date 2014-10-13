#include <FL/Fl.h>
#include "Mainframe.h"


/*
 * call main frame and start prog
 *
 */

int main()
{
	Fl::visual(FL_RGB);
	fl_register_images();
	Mainframe frame;

	return Fl::run();
}