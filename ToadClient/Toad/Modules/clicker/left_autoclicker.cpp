#include "pch.h"
#include "Toad/Toad.h"
#include "left_autoclicker.h"

using namespace toad;

namespace toadll {

void CLeftAutoClicker::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	/// True when first click
	static bool is_starting_click = false;

	/// True when already using the autoclicker
	static bool is_already_clicking = false;

	/// randomized time in ms that the right button (for blocking) is held
	static int block_hit_mdown_rand_ms = left_clicker::block_hit_ms;
	static bool block_hit_timer_started = false;
	static bool block_hit_allowed = true; 
	static Timer block_hit_timer;

	// TODO: test trade assist 
	static Timer trade_assist_timer;

	/// a delay before breaking a block after aiming at one
	static Timer break_blocks_timer;


	if (!left_clicker::enabled)
	{
		SLEEP(250);
		return;
	}

	// also checking outside of autoclicking
	if (block_hit_timer_started)
	{
		if (static_cast<int>(block_hit_timer.Elapsed<>()) >= block_hit_mdown_rand_ms)
		{
			right_mouse_up();
			block_hit_timer_started = false;
		}
	}

	if (GetForegroundWindow() == g_hWnd && GetAsyncKeyState(VK_LBUTTON) && !CVarsUpdater::IsInGui)
	{
		m_start = std::chrono::high_resolution_clock::now();

		m_pTick = CVarsUpdater::PartialTick;
		static auto enemy = MC->getMouseOverPlayer();
		if (!enemy)
		{
			enemy = MC->getMouseOverPlayer();
		}
		else
		{
			auto ePos = enemy->getPosition();
			auto yawDiff = std::abs(wrap_to_180(-(lPlayer->Yaw - get_angles(lPlayer->Pos, ePos).first)));

			// enemy is not valid anymore after it has gone out of these limits
			if (ePos.dist(lPlayer->Pos) > 4.0f || yawDiff > 120)
				enemy = nullptr;
		}

		auto mouse_over_type = MC->getMouseOverTypeStr();
		const auto& held_item = lPlayer->HeldItem;

		if (!is_starting_click)
		{
			m_rand.edited_min = m_rand.min_delay;
			m_rand.edited_max = m_rand.max_delay;

			is_starting_click = true;
		}

		if (left_clicker::break_blocks && !is_already_clicking)
		{
			// spinlock while mouse is over block
			while (mouse_over_type == "BLOCK" && GetAsyncKeyState(VK_LBUTTON))
			{
				SLEEP(1);
				mouse_over_type = MC->getMouseOverTypeStr();
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
				m_rand.edited_min = std::lerp(m_rand.edited_min, m_rand.min_delay - 2.5f, m_pTick / 2);
				m_rand.edited_max = std::lerp(m_rand.edited_max, m_rand.max_delay - 2.5f, m_pTick / 3);
			}
			else
			{
				// higher delay
				m_rand.edited_min = std::lerp(m_rand.edited_min, m_rand.min_delay + 2.5f, m_pTick / 3);
				m_rand.edited_max = std::lerp(m_rand.edited_max, m_rand.max_delay + 2.5f, m_pTick / 2);
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

			//LOGDEBUG("ENEMY HITS: %d | PLAYER HITS: %d | TRADING: {}", enemy_hit_count, lplayer_hit_count, is_trading ? "Y" : "N");

			if (enemy != nullptr)
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
					m_rand.edited_min = std::lerp(m_rand.edited_min, m_rand.min_delay - 10.0f, m_pTick / 2);
					m_rand.edited_max = std::lerp(m_rand.edited_max, m_rand.max_delay - 10.0f, m_pTick / 3);
				}
				else
				{
					static bool is_player_hit = false;
					static bool is_enemy_hit = false;
					if (!is_enemy_hit && enemy->getHurtTime() <= 2)
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

					m_rand.edited_min = std::lerp(m_rand.edited_min, left_clicker::targeting_affects_cps ? m_rand.min_delay + 2.5f : m_rand.min_delay, m_pTick / 2);
					m_rand.edited_max = std::lerp(m_rand.edited_max, left_clicker::targeting_affects_cps ? m_rand.max_delay + 2.5f : m_rand.max_delay, m_pTick / 3);
				}
			}
			else
			{
				first_hit = true;
				start_timer_flag = false;
				enemy_hit_count = 0;
				lplayer_hit_count = 0;
				m_rand.edited_min = std::lerp(m_rand.edited_min, left_clicker::targeting_affects_cps ? m_rand.min_delay + 2.5f : m_rand.min_delay, m_pTick / 2);
				m_rand.edited_max = std::lerp(m_rand.edited_max, left_clicker::targeting_affects_cps ? m_rand.max_delay + 2.5f : m_rand.max_delay, m_pTick / 3);
			}
		}

		if (!mouse_down())
			return;

		if (left_clicker::block_hit)
		{
			if (held_item.find("sword") != std::string::npos && mouse_over_type == "ENTITY")
			{
				if (enemy != nullptr)
				{
					const auto hurtTime = enemy->getHurtTime();

					if (block_hit_allowed && hurtTime > 0 && !block_hit_timer_started)
					{
						// block
						right_mouse_down();

						block_hit_mdown_rand_ms = rand_int(left_clicker::block_hit_ms - 5, left_clicker::block_hit_ms + 5);
						block_hit_timer.Start();
						block_hit_timer_started = true;
						block_hit_allowed = false;
					}
					else
					{
						if (hurtTime <= 1)
							block_hit_allowed = true;
					}
				}
			}
		}

		is_already_clicking = true;

		if (left_clicker::break_blocks)
		{
			static bool break_blocks_flag = false;
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
					// reaction time in ms
					if (break_blocks_timer.Elapsed<>() > rand_int(50, 200))
					{
						break_blocks_flag = true;
					}
				}

				while (mouse_over_type == "BLOCK" && break_blocks_flag)
				{
					SLEEP(1);
					mouse_over_type = MC->getMouseOverTypeStr();
					m_start = std::chrono::high_resolution_clock::now();
				}

				// TODO: add extra delay for when leaving block before clicking
			}
			else
			{
				break_blocks_timer.Start();
				start_once = true;
				break_blocks_flag = false;
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
			for (auto& b : m_rand.boosts)
			{
				b.Reset();
			}
			is_starting_click = false;
		}

		SLEEP(50);
	}

}

void CLeftAutoClicker::PreUpdate()
{
}

Randomization& CLeftAutoClicker::GetRand()
{
	return m_rand;
}

bool CLeftAutoClicker::mouse_down()
{
	m_rand.delay = rand_float(m_rand.edited_min, m_rand.edited_max);

	apply_rand(m_rand.inconsistencies);

	m_end = std::chrono::high_resolution_clock::now();
	m_delay_compensation = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_start).count()) / 1000.0f;

	precise_sleep((m_rand.delay + m_rand.inconsistency_delay - m_delay_compensation) / 1000.f);

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
	m_rand.delay = rand_float(m_rand.edited_min, m_rand.edited_max);

	apply_rand(m_rand.inconsistencies2);

	m_end = std::chrono::high_resolution_clock::now();
	m_delay_compensation = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_start).count()) / 1000.0f;

	precise_sleep((m_rand.delay + m_rand.inconsistency_delay - m_delay_compensation) / 1000.f);

	POINT pt{};
	GetCursorPos(&pt);
	PostMessage(g_hWnd, WM_LBUTTONUP, 0, LPARAM((pt.x, pt.y)));

	m_start = std::chrono::high_resolution_clock::now();

	if (no_click_delay::enabled)
	{
		CNoClickDelay::Invoke(MC);
	}

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

void CLeftAutoClicker::SetDelays(int min_cps, int max_cps)
{
	m_rand.UpdateDelays(min_cps, max_cps);
}

}