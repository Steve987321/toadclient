#pragma once

#include <string_view>
#include <vector>

#ifdef TOAD_LOADER
#include "../../ToadClient/vendor/nlohmann/json.hpp"
#else
#include "nlohmann/json.hpp"
#endif

namespace config
{
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
	nlohmann::json SettingsToJson();

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
	///	@param file_ext the extension of the file 
	void SaveConfig(std::string_view path, std::string_view file_name, std::string_view file_ext = ".toad");

	/// Load a config from the clipboard 
	bool LoadConfigFromClipBoard();

	/// Save the current settings to the clipboard
	void SaveConfigToClipBoard(); 

	/// Returns a list of all possible configs for toad
	///
	///	@param path the directory we look in
	std::vector<ConfigFile> GetAllConfigsInDirectory(std::string_view path);
}

inline std::vector<std::string> split_string(const std::string& str, char splitter)
{
	std::stringstream ss(str);
	std::string split;
	std::vector<std::string> res;
	while (std::getline(ss, split, splitter))
	{
		res.emplace_back(std::move(split));
	}

	return res;
}
