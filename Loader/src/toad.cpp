#include "toad.h"

#include "../../ToadClient/nlohmann/json.hpp"

HANDLE hMapFile = nullptr;
constexpr int bufSize = 1000;

std::shared_mutex mutex;

bool toad::pre_init()
{
	utils::Twin_scan = std::thread(utils::Fwin_scan);
	return true;
}

bool initialized = false;
bool toad::init()
{
	if (initialized)return false;
	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, bufSize, L"ToadClientMappingObj");
	if (hMapFile == NULL)
		return false;

	LPVOID pMem = MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 0);
	if (pMem == NULL)
	{
		CloseHandle(hMapFile);
		return false;
	}

	memset(pMem, L'\0', bufSize);

	UnmapViewOfFile(pMem);
	tupdate_settings = std::thread([]
		{
			while (is_running)
			{
				update_settings();
				SLOW_SLEEP(100);
			}
		});

	initialized = true;
	return true;
}

void toad::update_settings()
{
	std::unique_lock lock(mutex);
	auto pMem = MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 0);
	if (pMem == NULL)
		std::cout << "[!!!] something went wrong calling MapViewOfFile(...)\n";

	using json = nlohmann::json;
	json data;

	data["aaenabled"] = aa::enabled;
	data["aadistance"] = aa::distance;
	data["aaspeed"] = aa::speed;
	
	std::stringstream ss;
	ss << data << "END";
	CopyMemory(pMem, ss.str().c_str(), ss.str().length());

	UnmapViewOfFile(pMem);
}

void toad::stop_all_threads()
{
	if (utils::Twin_scan.joinable()) utils::Twin_scan.join();
	if (tupdate_settings.joinable()) tupdate_settings.join();
}

void toad::clean_up()
{
	CloseHandle(hMapFile);
}

