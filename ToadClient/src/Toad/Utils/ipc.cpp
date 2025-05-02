#include "pch.h"
#include "ipc.h"

namespace Utils
{
	IPC::~IPC()
	{
		UnMapData();
		if (file_handle)
			CloseFileMap();
	}

	bool IPC::CreateMappedFile(std::string_view name)
	{
		file_handle = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, buf_size, name.data());
		if (!file_handle)
			return false;

		return true;
	}

	bool IPC::OpenMappedFile(std::string_view name)
	{
		file_handle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name.data());

		if (!file_handle)
			return false;

		return true;
	}

	void* IPC::MapData()
	{
		mapped_data = MapViewOfFile(file_handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		return mapped_data;
	}

	void IPC::UnMapData()
	{
		if (mapped_data)
			UnmapViewOfFile(mapped_data);
	}

	bool IPC::CloseFileMap()
	{
		BOOL res = CloseHandle(file_handle);
		file_handle = NULL;
		return res;
	}

}
