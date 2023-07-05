#include "pch.h"
#include "Toad/Toad.h"
#include "leftautoclicker.h"

using namespace toad;

namespace toadll {

void CLeftAutoClicker::Update(const std::shared_ptr<LocalPlayerT>& lPlayer)
{
	static bool is_starting_click = false;

	static bool break_blocks_flag = false;  // decides if the player is gonna hold down lmb
	static bool block_hit_timer_started = false;
	static bool is_already_clicking = false; // checks if we were already using the autoclicker

	static int block_hit_rand_ms = left_clicker::block_hit_ms;

	// TODO: test trade assist 
	static CTimer trade_assist_timer;
	static CTimer break_blocks_timer; // a delay before breaking a block after aiming at one
	static CTimer block_hit_timer;

	if (!left_clicker::enabled)
	{
		SLEEP(250);
		return;
	}

	// also check when not holding lmb
	if (block_hit_timer_started)
	{
		if (static_cast<int>(block_hit_timer.Elapsed<>()) >= block_hit_rand_ms)
		{
			right_mouse_up();
			block_hit_timer_started = false;
		}
	}

	if (GetForegroundWindow() == g_hWnd && GetAsyncKeyState(VK_LBUTTON) && !CVarsUpdater::IsInGui)
	{
		m_start = std::chrono::high_resolution_clock::now();

		m_pTick = CVarsUpdater::PartialTick;
		static auto enemy = CVarsUpdater::MouseOverPlayer;
		static bool has_active_enemy = false;
		if (!CVarsUpdater::IsMouseOverPlayer)
		{
			enemy = CVarsUpdater::MouseOverPlayer;
			has_active_enemy = false;
		}
		else
		{
			auto yawDiff = std::abs(wrap_to_180(-(lPlayer->Yaw - get_angles(lPlayer->Pos, enemy.Pos).first)));
			has_active_enemy = enemy.Pos.dist(lPlayer->Pos) > 4.0f || yawDiff > 120;
		}

		auto mouse_over_type = get_mouse_over_type();
		const auto& held_item = lPlayer->HeldItem;

		if (!is_starting_click)
		{
			rand.edited_min = rand.min_delay;
			rand.edited_max = rand.max_delay;

			is_starting_click = true;
		}

		if (left_clicker::break_blocks && !is_already_clicking)
		{
			while (mouse_over_type == "BLOCK" && GetAsyncKeyState(VK_LBUTTON))
			{
				SLEEP(1);
				mouse_over_type = get_mouse_over_type();
				m_start = std::chrono::high_resolution_clock::now();
			}
		}

		if (left_clicker::weapons_only)
		{
			if (held_item.find("sword") == std::string::npos)
				return;
		}

		if (left_clicker::targeting_affects_cps)
		{
			if (mouse_over_type == "ENTITY")
			{
				// lower delay
				rand.edited_min = std::lerp(rand.edited_min, rand.min_delay - 2.5f, m_pTick / 2);
				rand.edited_max = std::lerp(rand.edited_max, rand.max_delay - 2.5f, m_pTick / 3);
			}
			else
			{
				// higher delay
				rand.edited_min = std::lerp(rand.edited_min, rand.min_delay + 2.5f, m_pTick / 3);
				rand.edited_max = std::lerp(rand.edited_max, rand.max_delay + 2.5f, m_pTick / 2);
			}
		}

		if (left_clicker::trade_assist)
		{
			static bool start_timer_flag = false;
			static bool is_trading = false;

			static bool first_hit = true;

			static int enemy_hit_count = 0;
			static int lplayer_hit_count = 0;
			static int trade_count_threshold = 3;

			log_Debug("ENEMY HITS: %d | PLAYER HITS: %d | TRADING: %s", enemy_hit_count, lplayer_hit_count, is_trading ? "Y" : "N");

			if (has_active_enemy)
			{
				if (first_hit)
				{
					first_hit = false;
					return;
				}

				if (!start_timer_flag)
				{
					trade_assist_timer.Start();
					start_timer_flag = true;
				}

				if (trade_assist_timer.Elapsed<>() > 1000)
				{
					const auto diff = std::abs(enemy_hit_count - lplayer_hit_count);
					if (diff <= 2 && std::max(enemy_hit_count, lplayer_hit_count) > trade_count_threshold)
					{
						is_trading = true;
					}
					else
					{
						lplayer_hit_count = 0;
						enemy_hit_count = 0;
						is_trading = false;
					}
					trade_assist_timer.Start();
				}

				if (is_trading)
				{
					rand.edited_min = std::lerp(rand.edited_min, rand.min_delay - 10.0f, m_pTick / 2);
					rand.edited_max = std::lerp(rand.edited_max, rand.max_delay - 10.0f, m_pTick / 3);
				}
				else
				{
					static bool is_player_hit = false;
					static bool is_enemy_hit = false;
					if (!is_enemy_hit && enemy.HurtTime <= 2)
					{
						enemy_hit_count++;
						is_enemy_hit = true;
					}
					else
						is_enemy_hit = false;

					if (!is_player_hit && lPlayer->HurtTime > 0)
					{
						lplayer_hit_count++;
						is_player_hit = true;
					}
					else
						is_player_hit = false;

					rand.edited_min = std::lerp(rand.edited_min, left_clicker::targeting_affects_cps ? rand.min_delay + 2.5f : rand.min_delay, m_pTick / 2);
					rand.edited_max = std::lerp(rand.edited_max, left_clicker::targeting_affects_cps ? rand.max_delay + 2.5f : rand.max_delay, m_pTick / 3);
				}
			}
			else
			{
				first_hit = true;
				start_timer_flag = false;
				enemy_hit_count = 0;
				lplayer_hit_count = 0;
				rand.edited_min = std::lerp(rand.edited_min, left_clicker::targeting_affects_cps ? rand.min_delay + 2.5f : rand.min_delay, m_pTick / 2);
				rand.edited_max = std::lerp(rand.edited_max, left_clicker::targeting_affects_cps ? rand.max_delay + 2.5f : rand.max_delay, m_pTick / 3);
			}
		}

		if (!mouse_down())
			return;

		is_already_clicking = true;

		if (left_clicker::break_blocks)
		{
			static bool start_once = false;

			if (mouse_over_type == "BLOCK")
			{
				if (!start_once)
				{
					break_blocks_timer.Start();
					start_once = true;
				}
				else
				{
					// reaction time of 200 ms
					if (break_blocks_timer.Elapsed<>() > 200)
					{
						break_blocks_flag = true;
					}
				}

				while (mouse_over_type == "BLOCK" && break_blocks_flag)
				{
					SLEEP(1);
					mouse_over_type = get_mouse_over_type();
					m_start = std::chrono::high_resolution_clock::now();
				}
			}
			else
			{
				break_blocks_timer.Start();
				start_once = true;
				break_blocks_flag = false;
			}
		}

		if (left_clicker::block_hit)
		{
			if (held_item.find("sword") != std::string::npos && mouse_over_type == "ENTITY")
			{
				if (has_active_enemy)
				{
					if (enemy.HurtTime <= 2 && !block_hit_timer_started)
					{
						// block
						right_mouse_down();

						block_hit_rand_ms = rand_int(left_clicker::block_hit_ms - 5, left_clicker::block_hit_ms + 5);
						block_hit_timer.Start();
						block_hit_timer_started = true;
					}
				}
			}
		}

		mouse_up();
	}
	else
	{
		if (!GetAsyncKeyState(VK_LBUTTON))
		{
			is_already_clicking = false;
		}
		if (block_hit_timer_started)
		{
			if (static_cast<int>(block_hit_timer.Elapsed<>()) >= 10)
			{
				right_mouse_up();
				block_hit_timer_started = false;
			}
		}

		if (is_starting_click)
		{
			for (auto& b : rand.boosts)
			{
				b.Reset();
			}
			is_starting_click = false;
		}

		SLEEP(50);
	}

}

bool CLeftAutoClicker::mouse_down()
{
	rand.delay = rand_float(rand.edited_min, rand.edited_max);

	apply_rand(rand.inconsistencies);

	m_end = std::chrono::high_resolution_clock::now();
	m_delay_compensation = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_start).count()) / 1000.0f;

	preciseSleep((rand.delay + rand.inconsistency_delay - m_delay_compensation) / 1000.f);

	if (!GetAsyncKeyState(VK_LBUTTON))
		return false;

	POINT pt{};
	GetCursorPos(&pt);
	PostMessage(g_hWnd, WM_LBUTTONDOWN, MKF_LEFTBUTTONDOWN, LPARAM((pt.x, pt.y)));

	m_start = std::chrono::high_resolution_clock::now();
	update_rand_vars();
	return true;
}

void CLeftAutoClicker::mouse_up()
{
	rand.delay = rand_float(rand.edited_min, rand.edited_max);

	apply_rand(rand.inconsistencies2);

	m_end = std::chrono::high_resolution_clock::now();
	m_delay_compensation = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_start).count()) / 1000.0f;

	preciseSleep((rand.delay + rand.inconsistency_delay - m_delay_compensation) / 1000.f);

	POINT pt{};
	GetCursorPos(&pt);
	PostMessage(g_hWnd, WM_LBUTTONUP, 0, LPARAM((pt.x, pt.y)));

	m_start = std::chrono::high_resolution_clock::now();
	update_rand_vars();
}

void CLeftAutoClicker::right_mouse_down()
{
	POINT pt{};
	GetCursorPos(&pt);
	PostMessage(g_hWnd, WM_RBUTTONDOWN, MKF_RIGHTBUTTONDOWN, LPARAM((pt.x, pt.y)));
}

void CLeftAutoClicker::right_mouse_up()
{
	POINT pt{};
	GetCursorPos(&pt);
	PostMessage(g_hWnd, WM_RBUTTONUP, 0, LPARAM((pt.x, pt.y)));
}

// TODO: instead of returning a string return an enum of object types
std::string CLeftAutoClicker::get_mouse_over_type() const
{
	auto str = Minecraft->getMouseOverBlockStr();
	auto start = str.find("type=") + 5;
	if (start == std::string::npos) return "";
	auto end = str.find(',', start);
	return str.substr(start, end - start);
}

void CLeftAutoClicker::SetDelays(int cps)
{
	rand.min_delay = (1000.f / cps - 2) / 2;
	rand.max_delay = (1000.f / cps + 2) / 2;
}

}