#pragma once

#include <string_view>
#include <vector>
#include <set>
#include <iostream>

#ifdef TOAD_LOADER
#include "../../ToadClient/vendor/nlohmann/json.hpp"
#else
#include "nlohmann/json.hpp"
#endif

namespace config
{
	inline std::set<std::string> logged_invalid_keys{};

	template<typename T>
	bool get_json_element(T& val, const nlohmann::json& data, std::string_view key) noexcept
	{
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

	struct ConfigFile
	{
		std::string FileNameStem;
		std::filesystem::file_time_type LastWrite;
	};

	/// Sets the current settings of a json string 
	bool LoadSettings(std::string_view jsonSettings, std::string& error_msg);

	/// Returns the combined data of a and b 
	nlohmann::json MergeJson(const nlohmann::json& a, const nlohmann::json& b);

	/// Returns a json of the current settings
	nlohmann::json SettingsToJson(bool include_keybinds = true);

	/// Load a config from a file
	///
	///	@param path the directory the file is in
	///	@param file_name the name of the file
	///	@param file_ext the extension of the file
	void LoadConfig(std::string_view path, std::string_view file_name, std::string_view file_ext = ".toad");

	/// Save the current settings to a file
	///
	///	@param path the directory to save to
	///	@param file_name the name of the file
	/// @param save_keybinds whether to save keybinds to file
	///	@param file_ext the extension of the file 
	void SaveConfig(std::string_view path, std::string_view file_name, bool save_keybinds = true, std::string_view file_ext = ".toad");

	/// Load a config from the clipboard 
	bool LoadConfigFromClipBoard();

	/// Save the current settings to the clipboard
	void SaveConfigToClipBoard(); 

	/// Returns a list of all possible configs for toad
	///
	///	@param path the directory we look in
	std::vector<ConfigFile> GetAllConfigsInDirectory(std::string_view path);
}
