#include "toad.h"

bool toad::init_toad()
{
	utils::twin_scan = std::thread(utils::fwin_scan_thread);
	return true;
}

void toad::stop_all_threads()
{
	if (utils::twin_scan.joinable()) utils::twin_scan.join();
}

