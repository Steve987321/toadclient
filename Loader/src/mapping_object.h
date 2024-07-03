#pragma once

namespace toad
{

// synchronized ipc using file mapping objects
class MappingObject
{
public:
	explicit MappingObject(DWORD size, std::string name);
	~MappingObject();

public:
	bool Create();
	
	const std::string_view Open();

	void Resize(DWORD size);

	
private:
	HANDLE m_fileHandle{};
	DWORD m_size = 0;
	LPVOID m_mapped = nullptr;
	std::string m_name;
};

}