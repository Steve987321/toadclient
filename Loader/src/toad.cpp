#include "toad.h"

bool toad::init_toad()
{
	utils::Twin_scan = std::thread(utils::Fwin_scan);
	return true;
}

void toad::stop_all_threads()
{
	if (utils::Twin_scan.joinable()) utils::Twin_scan.join();
}

