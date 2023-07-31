#include "pch.h"
#include "Toad/Toad.h"
#include "bridge_assist.h"

using namespace toad;

namespace toadll
{

void CBridgeAssist::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	// keep track of player sneaking
	// will help against being stuck on sneaking
	bool m_isSneaking = (bool)GetAsyncKeyState(VK_SHIFT);

	if (!bridge_assist::enabled)
	{
		// when disabling this cheat module, make sure we aren't sneaking anymore
		if (m_isSneaking)
		{
			UnSneak();
		}
		SLEEP(100);
		return;
	}

	m_from = lPlayer->Pos;
	m_from.y += 1;
	Vec3 hitBlockPos = {};

	// trace down exactly under the player 
	auto res = MC->rayTraceBlocks(m_from, m_direction, hitBlockPos, false);

	// the vertical diff between the player and block under the player
	auto diffY = lPlayer->Pos.y - hitBlockPos.y;
	diffY -= 1;

	if (lPlayer->Pitch < bridge_assist::pitch_check)
	{
		if (m_isSneaking && diffY == 0)
			UnSneak();

		SLEEP(1);
		return;
	}

	if (diffY != 0 && diffY <= bridge_assist::block_check)
	{
		SLEEP(1);
		return;
	}

	static bool jumped = false;

	m_prev = m_isEdge;
	m_isEdge = false;

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		jumped = true;
	}

	if (jumped)
	{
		UnSneak();

		// check if we are back on ground or going down
		if (lPlayer->Motion.y < -0.4f || diffY == 0.0f)
			jumped = false;
		return;
	}

	// check if vertical speed is too high
	bool isFalling = std::abs(lPlayer->Motion.y) > 0.5f;

	if (!isFalling)
	{
		if (static_cast<int>(diffY) != 0 || res == Minecraft::RAYTRACE_BLOCKS_RESULT::NO_HIT)
		{
			Sneak();
		}
		else
		{
			UnSneak();
		}
	}
	else
	{
		UnSneak();
	}

	SLEEP(1);
}

}
