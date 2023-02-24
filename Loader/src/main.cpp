#include "toad.h"
#include "Application/Application.h"

int main()
{
	auto p_App = std::make_unique<toad::c_Application>();

	// init window & toad
	if (!p_App->Init())
		return 1;

	// main loop 
	p_App->MainLoop();

	// clean up and exit 
	p_App->Exit();

	return 0;
}