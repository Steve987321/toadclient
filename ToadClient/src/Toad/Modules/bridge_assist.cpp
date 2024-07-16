#include "pch.h"
#include "Toad/toadll.h"
#include "bridge_assist.h"

using namespace toad;

namespace toadll
{

CBridgeAssist::CBridgeAssist()
{
	Enabled = &bridge_assist::enabled;
}

void CBridgeAssist::PreUpdate()
{
	WaitIsEnabled();
	WaitIsVerified();
	SLEEP(10);
}

void CBridgeAssist::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	// keep track of player sneaking
	// will help against being stuck on sneaking
	bool isSneaking = (bool)GetAsyncKeyState(VK_SHIFT);

	if (!*Enabled)
	{
		// when disabling this cheat module, make sure we aren't sneaking anymore
		if (isSneaking)
		{
			UnSneak();
		}
		m_has_pressed_shift = false;
		SLEEP(100);
		return;
	}

	// makes sure player has block in hand
	if (lPlayer->HeldItem.find("tile") == std::string::npos)
	{
		if (isSneaking)
		{
			UnSneak();
		}
		m_has_pressed_shift = false;
		SLEEP(50);
		return;		
	}

	if (bridge_assist::only_initiate_when_sneaking)
	{
		if (!m_has_pressed_shift && isSneaking)
		{
			m_has_pressed_shift = true;
		}
	}

	//const bool standingOnBlock = diffY <= FLT_EPSILON;

	if (lPlayer->Pitch < bridge_assist::pitch_check)
	{
		if (isSneaking)
			UnSneak();

		m_has_pressed_shift = false;

		SLEEP(1);
		return;
	}


	if (bridge_assist::only_initiate_when_sneaking && !m_has_pressed_shift)
	{
		SLEEP(1);
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

	if (diffY != 0 && diffY <= bridge_assist::block_check)
	{
		SLEEP(1);
		return;
	}

	static bool jumped = false;

	m_prev = m_is_edge;
	m_is_edge = false;

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

	SLEEP(20);
}

void CBridgeAssist::Sneak()
{
	m_is_edge = true;
	if (!m_prev)
	{
		send_key(VK_SHIFT, true);
		m_prev = true;
	}
}

void CBridgeAssist::UnSneak()
{
	m_is_edge = false;
	if (m_prev)
	{
		send_key(VK_SHIFT, false);
		m_prev = false;
	}
}

}
