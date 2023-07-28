#include "pch.h"
#include "Toad/Toad.h"
#include "auto_bridge.h"

namespace toadll
{

void CAutoBridge::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	auto res = MC->rayTraceBlocks(lPlayer->Pos, { 0, -1, 0 });
	std::cout << "raytrace block: " << res << std::endl;

	SLEEP(1);
}

}