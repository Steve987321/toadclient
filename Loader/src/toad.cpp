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
	// setup ipc 
	if (initialized) return false;
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

	// update settings for ipc 
	Tupdate_settings = std::thread([]
		{
			while (g_is_running)
			{
				Fupdate_settings();
				SLOW_SLEEP(100);
			}
		});

	initialized = true;
	return true;
}

void toad::Fupdate_settings()
{
	std::unique_lock lock(mutex);
	auto pMem = MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 0);
	if (pMem == NULL)
		std::cout << "[!] something went wrong calling MapViewOfFile\n";

	using json = nlohmann::json;
	json data;

	// aim assist
	data["aaenabled"] = aa::enabled;
	data["aadistance"] = aa::distance;
	data["aaspeed"] = aa::speed;
	data["aahorizontal_only"] = aa::horizontal_only;
	data["aafov"] = aa::fov;
	data["aainvisibles"] = aa::invisibles;
	data["aatargetFOV"] = aa::targetFOV;
	data["aaalways_aim"] = aa::always_aim;
	data["aamultipoint"] = aa::aim_at_closest_point;
	data["aalockaim"] = aa::lock_aim;

	// auto bridge
	data["abenabled"] = auto_bridge::enabled;
	data["abpitch_check"] = auto_bridge::pitch_check;

	// velocity
	data["velenabled"] = velocity::enabled;
	data["velhorizontal"] = velocity::horizontal;
	data["velvertical"] = velocity::vertical;
	data["velchance"] = velocity::chance;
	data["veldelay"] = velocity::delay;

	std::stringstream ss;
	ss << data << "END";
	OutputDebugStringA(ss.str().c_str());
	memcpy(pMem, ss.str().c_str(), ss.str().length());

	UnmapViewOfFile(pMem);
}

void toad::stop_all_threads()
{
	if (utils::Twin_scan.joinable()) utils::Twin_scan.join();
	if (Tupdate_settings.joinable()) Tupdate_settings.join();
}

void toad::clean_up()
{
	CloseHandle(hMapFile);
}

