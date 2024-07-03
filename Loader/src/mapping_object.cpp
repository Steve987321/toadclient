#include "toad.h"
#include "mapping_object.h"

namespace toad
{

	MappingObject::MappingObject(DWORD size, std::string name)
		: m_size(size), m_name(std::move(name))
	{
	}

	MappingObject::~MappingObject()
	{
		CloseHandle(m_fileHandle);
	}

	bool MappingObject::Create()
	{
		m_fileHandle = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, ipc_bufsize, m_name.c_str());
		if (m_fileHandle == NULL)
		{
			return false;
		}
		return true; 
	}

	const std::string_view MappingObject::Open()
	{
		m_mapped = MapViewOfFile(m_fileHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		if (m_mapped == NULL)
		{
			return {};
		}

		return { (LPCSTR)m_mapped, m_size };
	}

	void MappingObject::Resize(DWORD size)
	{
		m_size = size;
		CloseHandle(m_fileHandle);
	}

}