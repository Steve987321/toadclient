#include "pch.h"
#include "Toad/Toad.h"
#include "vars_updater.h"

void toadll::CVarsUpdater::Update(const std::shared_ptr<c_Entity>& lPlayer)
{
	RenderPartialTick = Minecraft->get_renderPartialTick();
	PartialTick = Minecraft->get_partialTick();
	PlayerList = Minecraft->get_playerList();
	SLOW_SLEEP(1);
}
