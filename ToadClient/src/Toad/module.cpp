#include "pch.h"
#include "Toad/toad.h"
#include "module.h"

namespace toad
{

void CModule::AddModule(CModule* mod)
{
	ModuleInstances.emplace_back(mod);
}

void CModule::UpdateModuleEnableStates()
{
	for (auto& m : ModuleInstances)
		m->UpdateEnabledState();
}

void CModule::SetEnv(JNIEnv* Env)
{
	env = Env;
}

void CModule::SetMC(Minecraft& mc)
{
	MC = &mc;
}

void CModule::UpdateEnabledState()
{
	std::lock_guard lock(enabled_update_mutex);
	if (EnabledPrev != Enabled)
		EnabledCV.notify_one();

	EnabledPrev = Enabled;
}

void CModule::PreUpdate()
{
	SLEEP(5);
}

void CModule::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	SLEEP(100);
}

void CModule::OnRender()
{
	// don't sleep 
}

void CModule::OnImGuiRender(ImDrawList* draw)
{
	// don't sleep 
}

void CModule::WaitIsVerified()
{
	std::unique_lock lock(verified_mutex);
	CVarsUpdater::IsVerifiedCV.wait(lock, [&] { return !g_is_running || CVarsUpdater::IsVerified; });
}

void CModule::WaitIsEnabled()
{
	std::unique_lock lock(enabled_mutex);
	EnabledCV.wait(lock, [&]{ return !g_is_running || Enabled; });
}

}
