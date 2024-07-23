#pragma once

namespace toadll
{

class Hook
{
public:
	Hook();

	static inline std::vector<Hook*> hookInstances = {};

public:
	bool IsNull() const;
	virtual void Enable();
	virtual void Disable();
	virtual void Dispose();

public:
	virtual bool Init() = 0;

public:
	/// calls Enable() on all hook instances
	static void EnableAllHooks();

	/// calls Disable() on all hook instances
	static void DisableAllHooks();

	/// calls Dispose() on all hook instances that are valid
	static void CleanAllHooks();

	/// calls Init() on all hook instances
	static void InitializeAllHooks();

protected:
	bool m_isHookEnabled = false;

	// pointer to original adress of hook 
	void* m_oPtr = nullptr;

protected:
	static inline bool isMHInitialized = false;

protected:
	bool create_hook(const char* modName, const char* procName, void* detour, void** original);

};

}
