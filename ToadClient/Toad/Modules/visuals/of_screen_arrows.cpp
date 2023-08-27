#include "pch.h"
#include "Toad/toad.h"
#include "of_screen_arrows.h"

using namespace toadll::math;

void toadll::COfScreenArrows::PreUpdate()
{
	CModule::PreUpdate();
}

void toadll::COfScreenArrows::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	for (const auto& e : MC->getPlayerList())
	{
		auto pos = e->getPosition();
		if (pos.dist(lPlayer->Pos) < 20 /* setting */)
		{
			auto screen = world_to_screen(pos, get_cam_pos(CVarsUpdater::ModelView));
			if (screen.x < 0 || screen.x > g_screen_width)
			{
				
			}
		}
	}
	SLEEP(10);
}

void toadll::COfScreenArrows::OnImGuiRender(ImDrawList* draw)
{
	
}

void toadll::COfScreenArrows::OnRender()
{
	
}
