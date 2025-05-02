#pragma once 

#include "Toad/toad.h"
#include <functional>

namespace Injector
{
	inline std::string inject_status; 
	TOAD_API bool Inject(DWORD pid, const std::filesystem::path& dll_path, std::function<void(const std::string&)> callback);
}
