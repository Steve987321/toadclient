#include "pch.h"
#include "Toad/Toad.h"
#include "vars_updater.h"

void toadll::CVarsUpdater::PreUpdate()
{
	auto world = MC->getWorld();
	auto localPlayer = MC->getLocalPlayer();
	if (world == nullptr || localPlayer == nullptr)
	{
		IsVerified = false;

		if (world != nullptr)
			env->DeleteLocalRef(world);

		SLEEP(20);
		return;
	}

	env->DeleteLocalRef(world);

	//static auto lPlayerName = localPlayer->getName();
	//theLocalPlayer->Name = lPlayerName;
	theLocalPlayer->obj = localPlayer->obj;
	theLocalPlayer->Health = localPlayer->getHealth();
	theLocalPlayer->Invis = localPlayer->isInvisible();
	theLocalPlayer->Pos = localPlayer->getPosition();
	theLocalPlayer->HeldItem = localPlayer->getHeldItemStr();
	theLocalPlayer->HurtTime = localPlayer->getHurtTime();
	theLocalPlayer->LastTickPos = localPlayer->getLastTickPosition();
	theLocalPlayer->Pitch = localPlayer->getRotationPitch();
	theLocalPlayer->Yaw = localPlayer->getRotationYaw();
	theLocalPlayer->Motion = { localPlayer->getMotionX(), localPlayer->getMotionY(), localPlayer->getMotionZ() };

	if (auto tmpMouseOver = MC->getMouseOverPlayer(); tmpMouseOver != nullptr)
	{
		Entity e {};
		e.Pos = tmpMouseOver->getPosition();
		e.HurtTime = tmpMouseOver->getHurtTime();
		MouseOverPlayer = e;

		//MouseOverPlayer.Pos = tmpMouseOver->getPosition();
		//MouseOverPlayer.HurtTime = tmpMouseOver->getHurtTime();
		// add other properties when needed ...

		IsMouseOverPlayer = true;
	}
	else
	{
		IsMouseOverPlayer = false;
	}

	SLEEP(1);

	IsVerified = true;

}

void toadll::CVarsUpdater::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	static auto ari = MC->getActiveRenderInfo();
	ari->set_modelview(ModelView);
	ari->set_projection(Projection);
	RenderPartialTick = MC->getRenderPartialTick();
	PartialTick = MC->getPartialTick();
	IsInGui = MC->isInGui();
}
