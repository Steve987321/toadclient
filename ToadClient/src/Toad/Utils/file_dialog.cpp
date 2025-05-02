#include "pch.h"
#include "Toad/toad.h"
#include "Toad/Utils/file_dialog.h"
#include <commdlg.h>

std::string FileDialogGetFile(const std::filesystem::path& path, std::vector<std::string>& file_types)
{
	OPENFILENAMEA ofn = { 0 };
	char selected_file[MAX_PATH]{};
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = selected_file;
	ofn.nMaxFile = MAX_PATH;

	std::string file_types_str;
	for (const std::string& ext : file_types) 
		file_types_str += ext + " (*" + ext + ")\0*" + ext + "\0";
	
	file_types_str += "\0"; 

	//DWORD f = 0;
	//f |= (bool)(flags & FileDialogFlags::ALLOW_MULTIPLE_SELECTION) ? OFN_ALLOWMULTISELECT : 0;
	//f |= OFN_EXPLORER; // better style 
	//ofn.Flags = f;

	ofn.lpstrInitialDir = path.string().c_str();

	if (!GetOpenFileNameA(&ofn))
		return "";

	return ofn.lpstrFile;
}
