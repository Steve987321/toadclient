#include "pch.h"
#include "Toad/Toad.h"
#include "vars_updater.h"

void toadll::CVarsUpdater::PreUpdate()
{
	static bool joiningWorld = false;

	auto world = MC->getWorld();
	auto localPlayer = MC->getLocalPlayer();
	if (world == nullptr || localPlayer == nullptr)
	{
		IsVerified = false;

		if (world != nullptr)
			env->DeleteLocalRef(world);

		joiningWorld = true;
		SLEEP(100);
		return;
	}

	env->DeleteLocalRef(world);

	// while joining world, do extra checks to prevent crashing 
	if (joiningWorld)
	{
		// wait a bit 
		SLEEP(500);

		// check again
		localPlayer = MC->getLocalPlayer();
		if (!localPlayer)
		{
			return;
		}
	}

	//static auto lPlayerName = localPlayer->getName();
	//theLocalPlayer->Name = lPlayerName;
	//theLocalPlayer->obj = localPlayer->obj;
	//theLocalPlayer->Health = localPlayer->getHealth();
	//theLocalPlayer->Invis = localPlayer->isInvisible();
	theLocalPlayer->Pos = localPlayer->getPosition();
	theLocalPlayer->HeldItem = localPlayer->getHeldItemStr();
	theLocalPlayer->HurtTime = localPlayer->getHurtTime();
	theLocalPlayer->LastTickPos = localPlayer->getLastTickPosition();
	theLocalPlayer->Pitch = localPlayer->getRotationPitch();
	theLocalPlayer->Yaw = localPlayer->getRotationYaw();
	theLocalPlayer->Motion = { localPlayer->getMotionX(), localPlayer->getMotionY(), localPlayer->getMotionZ() };

	joiningWorld = false;
	IsVerified = true;
	SLEEP(1);
}

void toadll::CVarsUpdater::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	static auto ari = MC->getActiveRenderInfo();
	ari->getModelView(ModelView);
	ari->getProjection(Projection);
	RenderPartialTick = MC->getRenderPartialTick();
	PartialTick = MC->getPartialTick();
	IsInGui = MC->isInGui();
}
