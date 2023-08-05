#include "pch.h"
#include "Toad/toad.h"
#include "of_screen_arrows.h"

void toadll::COfScreenArrows::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	std::vector<std::pair<Entity, Vec3>> res;
	for (const auto& e : GetPlayerList())
	{
		
	}
	m_screenPositions = res;
	SLEEP(1);
}

void toadll::COfScreenArrows::OnImGuiRender(ImDrawList* draw)
{
	
}

void toadll::COfScreenArrows::OnRender()
{
	
}
