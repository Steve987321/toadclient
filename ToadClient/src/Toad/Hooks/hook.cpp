#include "pch.h"
#include "Toad/toadll.h"
#include "hook.h"

namespace toadll
{
	Hook::Hook()
	{
		hookInstances.emplace_back(this);
	}

	bool Hook::IsNull() const
	{
		return m_oPtr == nullptr;
	}

	void Hook::Enable()
	{
		m_isHookEnabled = true;
		MH_EnableHook(m_oPtr);
	}

	void Hook::Disable()
	{
		m_isHookEnabled = false;
		MH_DisableHook(m_oPtr);
	}

	void Hook::Dispose()
	{
		if (m_isHookEnabled)
			MH_DisableHook(m_oPtr);
	}

	void Hook::EnableAllHooks()
	{
		for (const auto& hook : hookInstances)
			hook->Enable();
	}

	void Hook::DisableAllHooks()
	{
		for (const auto& hook : hookInstances)
			hook->Disable();
	}

	void Hook::CleanAllHooks()
	{
		for (const auto& hook : hookInstances)
			if (!hook->IsNull())
				hook->Dispose();
	}

	void Hook::InitializeAllHooks()
	{
		for (const auto& hook : hookInstances)
			hook->Init();
	}

	bool Hook::create_hook(const char* modName, const char* procName, void* detour, void** original)
	{
		if (!isMHInitialized)
		{
			if (MH_Initialize() != MH_OK)
			{
				LOGERROR("[Hook] failed to initialize minhook");
				return false;
			}
		}

		isMHInitialized = true;

		HMODULE mod = GetModuleHandleA(modName);
		if (!mod)
		{
			LOGERROR("[Hook] Failled to get module handle {}", modName);
			return false;
		}

		m_oPtr = static_cast<void*>(GetProcAddress(mod, procName));
		LOGDEBUG("[Hook] Creating hook modname: {} procname {} optr: {}", modName, procName, m_oPtr);

		if (!m_oPtr)
		{
			LOGDEBUG("[Hook] GetProcAddress return nullptr for procname {}", procName);
			return false;
		}

		return MH_CreateHook(m_oPtr, detour, original) == MH_OK;
	}

}
