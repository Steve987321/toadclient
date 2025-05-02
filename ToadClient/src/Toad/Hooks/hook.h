#pragma once

namespace toad
{

class Hook
{
public:
	static inline std::vector<Hook*> HookInstances = {};
	static void AddHook(Hook* hook);

	bool IsNull() const;
	virtual void Enable();
	virtual void Disable();
	virtual void Dispose();

	virtual bool Init();

	/// calls Enable() on all hook instances
	static void EnableAllHooks();

	/// calls Disable() on all hook instances
	static void DisableAllHooks();

	/// calls Dispose() on all hook instances that are valid
	static void CleanAllHooks();

	/// calls Init() on all hook instances
	static void InitializeAllHooks();

	template<typename T> 
	static T* GetInstance()
	{
		static T instance;
		return &instance;
	}

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

#define REGISTER_HOOK(THOOK) Hook::AddHook(THOOK::GetInstance<THOOK>())
