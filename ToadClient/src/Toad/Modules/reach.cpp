#include "pch.h"
#include "Toad/toadll.h"
#include "reach.h"

using namespace toadll::math;

void toadll::CReach::PreUpdate()
{
	SLEEP(1);
}

void toadll::CReach::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	SLEEP(1000);
	
}

void toadll::CReach::Invoke(const std::shared_ptr<LocalPlayer>& lPlayer, std::shared_ptr<Minecraft>& MC, JNIEnv* env)
{
	static std::shared_ptr<c_Entity> playerobj = nullptr;

	for (const auto& e : MC->getPlayerList())
	{
		float yaw_diff = abs(wrap_to_180(-(lPlayer->Yaw - get_angles(lPlayer->Pos, e->getPosition()).first)));
		if (yaw_diff < 2 && e->getPosition().dist(lPlayer->Pos) > 3)
		{
			playerobj = e;
			break;
		}
	}
}
