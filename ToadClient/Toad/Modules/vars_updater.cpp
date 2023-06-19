#include "pch.h"
#include "Toad/Toad.h"
#include "vars_updater.h"

void toadll::CVarsUpdater::PreUpdate()
{
	auto world = Minecraft->getWorld();
	auto tmpPlayer = Minecraft->getLocalPlayer();
	if (world == nullptr || tmpPlayer == nullptr)
	{
		if (world == nullptr)
			env->DeleteLocalRef(world);
		IsVerified = false;

		m_playerList = {};

		return;
	}

	env->DeleteLocalRef(world);

	//static auto lPlayerName = tmpPlayer->getName();
	//LocalPlayer->Name = lPlayerName;
	LocalPlayer->obj = tmpPlayer->obj;
	LocalPlayer->Health = tmpPlayer->getHealth();
	LocalPlayer->Invis = tmpPlayer->isInvisible();
	LocalPlayer->Pos = tmpPlayer->getPosition();
	LocalPlayer->HeldItem = tmpPlayer->getHeldItemStr();
	LocalPlayer->HurtTime = tmpPlayer->getHurtTime();
	LocalPlayer->LastTickPos = tmpPlayer->getLastTickPosition();
	LocalPlayer->Pitch = tmpPlayer->getRotationPitch();
	LocalPlayer->Yaw = tmpPlayer->getRotationYaw();
	LocalPlayer->Motion = { tmpPlayer->getMotionX(), tmpPlayer->getMotionY(), tmpPlayer->getMotionZ() };

	const auto entityList = Minecraft->getPlayerList();
	//std::vector<std::shared_ptr<EntityT>> tmp = {};
	//for (const auto& e : entityList)
	//{
	//	if (!e || !e->obj)
	//		continue;
	//	if (env->IsSameObject(LocalPlayer->obj, e->obj))
	//		continue;

	//	auto entity = std::make_shared<EntityT>();
	//	//entity->Name = eName;
	//	entity->obj = e->obj;
	//	entity->Pos = e->getPosition();
	//	entity->HurtTime = e->getHurtTime();
	//	entity->LastTickPos = e->getLastTickPosition();
	//	entity->Invis = e->isInvisible();
	//	entity->Pitch = e->getRotationPitch();
	//	entity->Yaw = e->getRotationYaw();
	//	tmp.emplace_back(entity);
	//}
	//
	std::vector<EntityT> tmp = {};
	for (const auto& e : entityList)
	{
		if (!e || !e->obj)
			continue;
		if (env->IsSameObject(LocalPlayer->obj, e->obj))
			continue;

		EntityT entity;
		//entity->Name = eName;
		entity.obj = e->obj;
		entity.Pos = e->getPosition();
		entity.HurtTime = e->getHurtTime();
		entity.LastTickPos = e->getLastTickPosition();
		entity.Invis = e->isInvisible();
		entity.Pitch = e->getRotationPitch();
		entity.Yaw = e->getRotationYaw();
		tmp.emplace_back(entity);
	}

	m_playerList = tmp;

	SLOW_SLEEP(1);
	IsVerified = true;

}

void toadll::CVarsUpdater::Update(const std::shared_ptr<LocalPlayerT>& lPlayer)
{
	static auto ari = Minecraft->getActiveRenderInfo();
	ari->set_modelview(ModelView);
	ari->set_projection(Projection);
	RenderPartialTick = Minecraft->getRenderPartialTick();
	PartialTick = Minecraft->getPartialTick();
	IsInGui = Minecraft->isInGui();

}
