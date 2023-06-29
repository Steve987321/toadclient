#include "toad.h"
#include "Application/Application.h"

int main()
{
	// init window & toad
	if (!toad::AppInstance->Init())
		return 1;

	// main loop 
	toad::AppInstance->MainLoop();

	// clean up and exit 
	toad::AppInstance->Exit();

	return 0;
}