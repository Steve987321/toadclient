#include "pch.h"
#include "Toad/Toad.h"
#include "leftautoclicker.h"

using namespace toad;

void toadll::CLeftAutoClicker::randomize_start()
{
	/*if (!rand.is_start_randomized)
	{
		if ((rand.start_rand_flag = rand_int(0, 1) == 0))
		{
			rand.edited_min -= rand.spike_amount;
			rand.edited_max -= rand.spike_amount;
		}
		else
		{
			rand.edited_min += rand.spike_amount;
			rand.edited_max += rand.spike_amount;
		}
		rand.is_start_randomized = true;
	}
	else
	{
		if (rand.start_rand_flag)
		{
			rand.edited_min -= rand.spike_amount;
			rand.edited_max -= rand.spike_amount;
		}
		else
		{
			rand.edited_min += rand.spike_amount;
			rand.edited_max += rand.spike_amount;
		}
	}*/
}

void toadll::CLeftAutoClicker::update_rand_vars()
{
	rand.inconsistency_delay = std::lerp(rand.inconsistency_delay, 0, m_pTick * 1.5f);
	//std::cout << rand.inconsistency_delay << std::endl;

	const int rand_100 = rand_int(0, 100);

	for (auto& i : rand.inconsistencies)
		if (!i.should_start && ++i.frequency_counter >= i.frequency)
		{
			if (i.chance >= rand_100)
				i.should_start = true;
			else
				i.frequency_counter -= static_cast<float>(i.frequency_counter) * (static_cast<float>(i.frequency_counter) / static_cast<float>(i.frequency) * 0.75f);
		}		

	for (auto& i2 : rand.inconsistencies2)
		if (!i2.should_start && ++i2.frequency_counter >= i2.frequency)
		{
			if (i2.chance <= rand_100)
				i2.should_start = true;
			else
				i2.frequency_counter -= static_cast<float>(i2.frequency_counter) * (static_cast<float>(i2.frequency_counter) / static_cast<float>(i2.frequency) * 0.75f);
		}

	for (auto& b : rand.boosts)
	{
		//log_Debug("%d: started: %s paused: %s counter: %d frequency_counter: %d", b.id, b.start ? "Y" : "N", b.paused ? "Y" : "N", b.counter, b.frequency_counter);

		if (b.paused)
			continue;

		if (!b.start && ++b.frequency_counter >= b.frequency)
		{
			b.start = true;
			// because we are boosting the cps we want to
			// make boosting less frequent and pause frequency counters for other boosters
			for (auto& b_other : rand.boosts)
			{
				if (b_other.id == b.id)
					continue;
				b_other.frequency_counter -= static_cast<float>(b_other.frequency_counter) * (static_cast<float>(b_other.frequency_counter) / static_cast<float>(b_other.frequency) * 0.75f);
				b_other.paused = true;
			}
		}

		b.frequency_counter++;
	}
		
}

void toadll::CLeftAutoClicker::apply_rand(std::vector<Inconsistency>& inconsistencies)
{
	for (auto& i : inconsistencies)
	{
		if (i.should_start)
		{
			rand.inconsistency_delay = rand_float(i.min_amount_ms, i.max_amount_ms);
			i.Reset();
			break;
		}
	}

	for (auto& boost : rand.boosts)
	{
		if (boost.start)
		{
			boost.counter++;
			// boost up
			if (boost.counter <= boost.transition_duration)
			{
				//log_Debug("id: %d boosting up (%f, %f)", boost.id, rand.edited_min, rand.edited_max);
				rand.edited_min -= boost.amount_ms;
				rand.edited_max -= boost.amount_ms / 2.0f;
			}

			// boost down 
			else if (boost.counter > boost.duration - boost.transition_duration
				&&
				boost.counter <= boost.duration)
			{
				//log_Debug("id: %d boosting down (%f, %f)", boost.id, rand.edited_min, rand.edited_max);
				rand.edited_min += boost.amount_ms;
				rand.edited_max += boost.amount_ms / 2.0f;
			}

			// reset this boost 
			else if (boost.counter > boost.duration)
			{
				boost.Reset();
				// unpause other boosts
				for (auto& b : rand.boosts)
					b.paused = false;
			}

			break; // make sure we dont apply other boosts while boosting 
		}
	}
}

void toadll::CLeftAutoClicker::mouse_down()
{
	rand.delay = rand_float(rand.edited_min, rand.edited_max);

	apply_rand(rand.inconsistencies);

	m_end = std::chrono::high_resolution_clock::now();
	m_delay_compensation = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_start).count()) / 1000.0f;

	preciseSleep((rand.delay + rand.inconsistency_delay - m_delay_compensation) / 1000.f);

	POINT pt{};
	GetCursorPos(&pt);
	PostMessage(g_hWnd, WM_LBUTTONDOWN, MKF_LEFTBUTTONDOWN, LPARAM((pt.x, pt.y)));

	m_start = std::chrono::high_resolution_clock::now();
	update_rand_vars();
}

void toadll::CLeftAutoClicker::mouse_up()
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

void toadll::CLeftAutoClicker::right_mouse_down()
{
	POINT pt{};
	GetCursorPos(&pt);
	PostMessage(g_hWnd, WM_RBUTTONDOWN, MKF_RIGHTBUTTONDOWN, LPARAM((pt.x, pt.y)));
}

void toadll::CLeftAutoClicker::right_mouse_up()
{
	POINT pt{};
	GetCursorPos(&pt);
	PostMessage(g_hWnd, WM_RBUTTONUP, 0, LPARAM((pt.x, pt.y)));
}


// TODO: instead of returning a string return an enum of object types
std::string toadll::CLeftAutoClicker::get_mouse_over_type() const
{
	auto str = Minecraft->get_mouseOverStr();
	auto start = str.find("type=") + 5;
	if (start == std::string::npos) return "";
	auto end = str.find(',', start);
	return str.substr(start, end - start);
}

void toadll::CLeftAutoClicker::SetDelays()
{
	rand.min_delay = (1000.f / clicker::cps - 2) / 2;
	rand.max_delay = (1000.f / clicker::cps + 2) / 2;
}

void toadll::CLeftAutoClicker::Update(const std::shared_ptr<c_Entity>& lPlayer)
{
	static bool is_starting_click = false;

	static bool break_blocks_flag = false; // decides if the player is gonna hold down lmb
	static bool block_hit_timer_started = false;

	static int block_hit_rand_ms = clicker::block_hit_ms;

	// TODO: test trade assist 
	static CTimer trade_assist_timer;
	static CTimer break_blocks_timer; // a delay before breaking a block after aiming at one
	static CTimer block_hit_timer;

	if (!clicker::enabled)
	{
		SLOW_SLEEP(250);
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

	if (GetForegroundWindow() == g_hWnd && GetAsyncKeyState(VK_LBUTTON) && !Minecraft->isInGui())
	{
		m_start = std::chrono::high_resolution_clock::now();

		m_pTick = Minecraft->get_partialTick();
		static auto enemy = Minecraft->get_mouseOverPlayer();
		if (enemy == nullptr)
		{
			enemy = Minecraft->get_mouseOverPlayer();
		}
		else if (enemy != nullptr)
		{
			auto yawDiff = std::abs(wrap_to_180(-(lPlayer->get_rotationYaw() - get_angles(lPlayer->get_position(), enemy->get_position()).first)));
			if (enemy->get_position().dist(lPlayer->get_position()) > 4.0f || yawDiff > 120)
				enemy = nullptr;
		}

		auto mouse_over_type = get_mouse_over_type();
		auto held_item = lPlayer->get_heldItemStr();

		if (!is_starting_click)
		{
			rand.edited_min = rand.min_delay;
			rand.edited_max = rand.max_delay;

			is_starting_click = true;
		}

		if (clicker::weapons_only)
		{
			if (held_item.find("sword") == std::string::npos)
				return;
		}

		if (clicker::targeting_affects_cps)
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

		if (clicker::trade_assist)
		{
			static bool start_timer_flag = false;
			static bool is_trading = false;

			static bool first_hit = true;

			static int enemy_hit_count = 0;
			static int lplayer_hit_count = 0;
			static int trade_count_threshold = 3;

			log_Debug("ENEMY HITS: %d | PLAYER HITS: %d | TRADING: %s", enemy_hit_count, lplayer_hit_count, is_trading ? "Y" : "N");

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
					rand.edited_min = std::lerp(rand.edited_min, rand.min_delay - 10.0f, m_pTick / 2);
					rand.edited_max = std::lerp(rand.edited_max, rand.max_delay - 10.0f, m_pTick / 3);
				}
				else
				{
					static bool is_player_hit = false;
					static bool is_enemy_hit = false;
					if (!is_enemy_hit && enemy->get_hurt_time() <= 2)
					{
						enemy_hit_count++;
						is_enemy_hit = true;
					}
					else
						is_enemy_hit = false;

					if (!is_player_hit && lPlayer->get_hurt_time() > 0)
					{
						lplayer_hit_count++;
						is_player_hit = true;
					}
					else
						is_player_hit = false;

					rand.edited_min = std::lerp(rand.edited_min, clicker::targeting_affects_cps ? rand.min_delay + 2.5f : rand.min_delay, m_pTick / 2);
					rand.edited_max = std::lerp(rand.edited_max, clicker::targeting_affects_cps ? rand.max_delay + 2.5f : rand.max_delay, m_pTick / 3);
				}
			}
			else
			{
				first_hit = true;
				start_timer_flag = false;
				enemy_hit_count = 0;
				lplayer_hit_count = 0;
				rand.edited_min = std::lerp(rand.edited_min, clicker::targeting_affects_cps ? rand.min_delay + 2.5f : rand.min_delay, m_pTick / 2);
				rand.edited_max = std::lerp(rand.edited_max, clicker::targeting_affects_cps ? rand.max_delay + 2.5f : rand.max_delay, m_pTick / 3);
			}
		}

		mouse_down();
		
		if (clicker::break_blocks)
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
					SLOW_SLEEP(1);
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

		if (clicker::block_hit)
		{
			if (held_item.find("sword") != std::string::npos && mouse_over_type == "ENTITY")
			{
				if (enemy != nullptr)
				{
					if (enemy->get_hurt_time() <= 2 && !block_hit_timer_started)
					{
						// block
						right_mouse_down();

						block_hit_rand_ms = rand_int(clicker::block_hit_ms - 5, clicker::block_hit_ms + 5);
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
		
		SLOW_SLEEP(10);
	}


}
