#include "pch.h"
#include "Application/application.h"

using namespace toad;

int main(int argc, char** argv)
{
	// init window & toad
	if (!Application::Init())
		return 1;

	// main loop 
	Application::MainLoop();

	// clean up and exit 
	Application::Exit();

	return 0;
}