#pragma once

#include "Toad/toad.h"

namespace Utils
{
	class TOAD_API IPC
	{
	public:
		~IPC();

		static inline size_t buf_size = 10000;

		// when writing
		bool CreateMappedFile(std::string_view name);

		// when reading
		bool OpenMappedFile(std::string_view name);
		void* MapData();
		void UnMapData();
		bool CloseFileMap();

	private:
		HANDLE file_handle;
		void* mapped_data = nullptr;
	};
}
