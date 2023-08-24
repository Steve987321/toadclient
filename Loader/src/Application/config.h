#pragma once

#include <string_view>
#include <vector>

#include "../../ToadClient/vendor/nlohmann/json.hpp"

namespace config
{
	struct ConfigFile
	{
		std::string FileNameStem;
		std::filesystem::file_time_type LastWrite;
	};

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