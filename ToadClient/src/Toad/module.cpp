#include "pch.h"
#include "Toad/toadll.h"
#include "module.h"

namespace toadll
{

CModule::CModule()
{
	ModuleInstances.emplace_back(this);
}

std::vector<CModule*> CModule::GetEnabledModules()
{
	std::vector<CModule*> res = {};
	for (const auto& m : ModuleInstances)
	{
		if (m->Enabled)
			res.push_back(m);
	}
	return res;
}

void CModule::SetEnv(JNIEnv* Env)
{
	env = Env;
}

void CModule::SetMC(std::unique_ptr<Minecraft>& mc)
{
	MC = std::move(mc);
}

void CModule::UpdateEnabledState()
{
	std::lock_guard lock(enabled_update_mutex);
	if (EnabledPrev != *Enabled)
	{
		EnabledCV.notify_one();
	}

	EnabledPrev = *Enabled;
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
