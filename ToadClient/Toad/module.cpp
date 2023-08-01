#include "pch.h"
#include "Toad/Toad.h"
#include "module.h"

namespace toadll
{

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
	SLEEP(1);
}

void CModule::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	SLEEP(1);
}

void CModule::OnRender()
{
	// don't sleep 
}

void CModule::OnImGuiRender(ImDrawList* draw)
{
	// don't sleep 
}

std::vector<Entity> CModule::GetPlayerList() const
{
	const auto playerList = MC->getPlayerList();

	std::vector<Entity> tmp = {};
	for (const auto& e : playerList)
	{
		if (!e || !e->obj)
			continue;
		if (env->IsSameObject(MC->getLocalPlayerObject(), e->obj))
			continue;

		Entity entity;
		entity.obj = e->obj;
		entity.Name = e->getName();
		entity.Pos = e->getPosition();
		entity.HurtTime = e->getHurtTime();
		entity.LastTickPos = e->getLastTickPosition();
		entity.Invis = e->isInvisible();
		entity.Pitch = e->getRotationPitch();
		entity.Yaw = e->getRotationYaw();
		tmp.emplace_back(entity);
	}

	return tmp;
}

}
