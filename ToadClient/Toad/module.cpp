#include "pch.h"
#include "Toad/Toad.h"
#include "module.h"

namespace toadll
{

std::vector<CModule*> CModule::GetEnabledModules()
{
	std::vector<CModule*> res = {};
	for (const auto m : moduleInstances)
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

}
