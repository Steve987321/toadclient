#include "pch.h"
#include "Toad/Toad.h"
#include "bridge_assist.h"

namespace toadll
{

void CBridgeAssist::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	m_from = lPlayer->Pos;
	m_from.y += 1;
	Vec3 hitBlockPos = {};
	auto res = MC->rayTraceBlocks(m_from, m_direction, hitBlockPos, false);

	// the diff between the player and block under the player
	auto diff = lPlayer->Pos - hitBlockPos;
	diff.y -= 1;

	bool prev = m_isEdge;
	static bool jumped = false;
	m_isEdge = false;

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		jumped = true;
	}

	if (jumped)
	{
		m_isEdge = false;
		if (prev)
		{
			send_key(VK_SHIFT, false);
			prev = false;
		}

		// check if we are back on ground or going down
		if (lPlayer->Motion.y < -0.4 || diff.y == 0)
			jumped = false;
		std::cout << diff.y << " " << lPlayer->Motion.y << std::endl;
		return;
	}

	// check if vertical speed is too high
	bool isFalling = std::abs(lPlayer->Motion.y) > 0.5f;

	if (!isFalling)
	{
		if (static_cast<int>(diff.y) != 0 || res == Minecraft::RAYTRACE_BLOCKS_RESULT::NO_HIT)
		{
			m_isEdge = true;
			if (!prev)
			{
				send_key(VK_SHIFT, true);
				prev = true;
			}
		}
		else
		{
			m_isEdge = false;
			if (prev)
			{
				send_key(VK_SHIFT, false);
				prev = false;
			}
		}
	}
	else
	{
		m_isEdge = false;
		if (prev)
		{
			send_key(VK_SHIFT, false);
			prev = false;
		}
	}
	SLEEP(1);
}

}
