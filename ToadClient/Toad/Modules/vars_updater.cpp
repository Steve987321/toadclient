#include "pch.h"
#include "Toad/Toad.h"
#include "vars_updater.h"

void toadll::CVarsUpdater::PreUpdate()
{
	auto world = Minecraft->get_world();
	auto tmpPlayer = Minecraft->get_localplayer();
	if (tmpPlayer == nullptr || world == nullptr)
	{
		if (world == nullptr)
			env->DeleteLocalRef(world);
		IsVerified = false;
		return;
	}
	IsVerified = true;
	
	env->DeleteLocalRef(world);
	
	static auto lPlayerName = tmpPlayer->get_name(); 
	LocalPlayer->Name = lPlayerName;
	LocalPlayer->obj = tmpPlayer->obj;
	LocalPlayer->Invis = tmpPlayer->is_invisible();
	LocalPlayer->Pos = tmpPlayer->get_position();
	LocalPlayer->HeldItem = tmpPlayer->get_heldItemStr();
	LocalPlayer->HurtTime = tmpPlayer->get_hurt_time();
	LocalPlayer->LastTickPos = tmpPlayer->get_lasttickposition();
	LocalPlayer->Pitch = tmpPlayer->get_rotationPitch();
	LocalPlayer->Yaw = tmpPlayer->get_rotationYaw();
	LocalPlayer->motion = { tmpPlayer->get_motionX(), tmpPlayer->get_motionY(), tmpPlayer->get_motionZ() };

	auto entityList = Minecraft->get_playerList();
	std::vector<std::shared_ptr<EntityT>> tmp = {};
	for (const auto& e : entityList)
	{
		auto eName = e->get_name();
		if (LocalPlayer->Name == eName)
			continue;

		auto entity = std::make_shared<EntityT>();
		entity->Name = eName;
		entity->obj = e->obj;
		entity->Pos = e->get_position();
		entity->HurtTime = e->get_hurt_time();
		entity->LastTickPos = e->get_lasttickposition();
		entity->Invis = e->is_invisible();
		entity->HeldItem = e->get_heldItemStr();
		entity->Pitch = e->get_rotationPitch();
		entity->Yaw = e->get_rotationYaw();
		tmp.emplace_back(entity);
	}

	PlayerList = tmp;

	SLOW_SLEEP(5);
}

void toadll::CVarsUpdater::Update(const std::shared_ptr<LocalPlayerT>& lPlayer)
{
	static auto ari = Minecraft->get_active_render_info();
	ModelView = ari->get_modelview();
	Projection = ari->get_projection();

	RenderPartialTick = Minecraft->get_renderPartialTick();
	PartialTick = Minecraft->get_partialTick();
	IsInGui = Minecraft->isInGui();
	SLOW_SLEEP(1);
}
