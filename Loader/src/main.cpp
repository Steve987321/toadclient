#include "toad.h"
#include "Application/Application.h"

int main()
{
	toad::p_App = std::make_unique<toad::c_Application>();

	// init window & toad
	if (!toad::p_App->Init())
		return 1;

	// main loop 
	toad::p_App->MainLoop();

	// clean up and exit 
	toad::p_App->Dispose();

	return 0;
}