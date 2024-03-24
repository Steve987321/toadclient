#include "toad.h"
#include "Application/application.h"

using namespace toad;

int main()
{
	Application app;
	
	// init window & toad
	if (!app.Init())
		return 1;

	// main loop 
	app.MainLoop();

	// clean up and exit 
	app.Exit();

	return 0;
}