#pragma once

#include "Toad/types.h"

#include <wtypes.h>
#include <random>
#include <filesystem>
#include <string_view>
#include <string>
#include <set>

#include "Toad/toad_defs.h"

#include "nlohmann/json.hpp"
#include "Toad/logger.h"

namespace toad
{
	
	extern Logger g_logger;

template<typename T>
bool GetJsonElement(T& val, const nlohmann::json& data, std::string_view key) noexcept
{
	static std::set<std::string> logged_invalid_keys{};

	if (data.contains(key) && !data.at(key).is_null())
	{
		try
		{
			val = data.at(key).get<T>();

			if (logged_invalid_keys.contains(key.data()))
			{
				logged_invalid_keys.erase(key.data());
			}
			return true;
		}
		catch (const nlohmann::json::exception& e)
		{
			if (logged_invalid_keys.contains(key.data()))
			{
				return false;
			}

			logged_invalid_keys.emplace(key);
#ifdef TOAD_LOADER
			std::cout << "[config] Failed to load property " << key.data() << ' ' << e.what() << std::endl;
#else
			LOGERROR("[config] Failed to load property {}, {}", key.data(), e.what());
#endif 
			return false;
		}
	}
	else
	{
		if (logged_invalid_keys.contains(key.data()))
		{
			return false;
		}

		logged_invalid_keys.emplace(key);
#ifdef TOAD_LOADER
		std::cout << "[config] Failed to load property: " << key.data() << " doesn't exist" << std::endl;
#else
		LOGERROR("[config] Failed to load property: {}, doesn't exist", key.data());
#endif 

		return false;
	}
}

TOAD_API float RandFloat(float min, float max);

TOAD_API int RandInt(int min, int max);

// Smooth interpolation  
TOAD_API float Slerp(float start, float end, float t);

// very precise
TOAD_API void PreciseSleep(double seconds);

/// Sends a keyboard key press
///
/// @param vk_key keycode to send
/// @param send_down whether we want to send the key down or up
TOAD_API void SendKey(WORD vk_key, bool send_down = true);

TOAD_API Vec3 GetClosestPoint(const BBox& bb, const Vec3& from);

/// Returns the directory location to the Documents folder 
TOAD_API std::filesystem::path GetDocumentsFolder();

}