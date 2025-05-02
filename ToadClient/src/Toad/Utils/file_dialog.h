#pragma once

#include <vector>
#include <filesystem>
#include "Toad/toad_defs.h"

TOAD_API std::string FileDialogGetFile(const std::filesystem::path& path, std::vector<std::string>& file_types);
