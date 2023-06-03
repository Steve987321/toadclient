#pragma once

#include "Toad/Types.h"
#include "Toad/MC/Utils/mcutils.h"

namespace toadll
{
class c_Hook;

inline bool mh_initialized = false;
inline std::vector<c_Hook*> hookInstances = {};

/**
 * @brief
 *
 */
class c_Hook
{
protected:
	bool enabled = false;
	void* optr = nullptr;

protected:
	bool create_hook(const char* moduleName, const char* procName, void* detour, void** original);

public:
	c_Hook();

public:
	bool is_null() const;
	void enable();
	void disable();
	void dispose() const;
};

}
