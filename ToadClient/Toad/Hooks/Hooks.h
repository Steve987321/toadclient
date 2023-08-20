#pragma once

#include "Toad/Types.h"
#include "Toad/MC/Utils/mcutils.h"

namespace toadll
{

class Hook
{
protected:
	bool m_isHookEnabled = false;

	// pointer to original adress of hook 
	void* m_oPtr = nullptr;

protected:
	static inline bool isMHInitialized = false;

protected:
	bool create_hook(const char* modName, const char* procName, void* detour, void** original)
	{
		if (!isMHInitialized)
			if (MH_Initialize() != MH_OK)
				return false;
		isMHInitialized = true;

		auto hMod = GetModuleHandleA(modName);
		if (!hMod)
		{
			return false;
		}

		m_oPtr = static_cast<void*>(GetProcAddress(hMod, procName));
		LOGDEBUG("[HOOK] [Creating hook] modName: {} procName {} optr: {}", modName, procName, m_oPtr);
		
		if (!m_oPtr)
		{
			return false;
		}

		return MH_CreateHook(m_oPtr, detour, original) == MH_OK;
	}

public:
	static inline std::vector<Hook*> hookInstances = {}; 

public:
	Hook()
	{
		hookInstances.emplace_back(this);
	}

public:
	_NODISCARD bool isNull() const
	{
		return m_oPtr == nullptr;
	}
	virtual void Enable()
	{
		m_isHookEnabled = true;
		MH_EnableHook(m_oPtr);
	}
	virtual void Disable()
	{
		m_isHookEnabled = false;
		MH_DisableHook(m_oPtr);
	}
	virtual void Dispose()
	{
		if (m_isHookEnabled)
			MH_DisableHook(m_oPtr);
	}

public:
	virtual bool Init() = 0;

public:
	/// calls Enable() on all hook instances
	static void EnableAllHooks()
	{
		for (const auto& hook : hookInstances)
			hook->Enable();
	}

	/// calls Disable() on all hook instances
	static void DisableAllHooks()
	{
		for (const auto& hook : hookInstances)
			hook->Disable();
	}

	/// calls Dispose() on all hook instances that are valid
	static void CleanAllHooks()
	{
		for (const auto& hook : hookInstances)
			if (!hook->isNull())
				hook->Dispose();
	}

	/// calls Init() on all hook instances
	static void InitializeAllHooks()
	{
		for (const auto& hook : hookInstances)
			hook->Init();
	}
};

}
