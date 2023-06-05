#include "pch.h"
#include "Toad/Toad.h"
#include "autoclicker.h"

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
	const int rand_100 = rand_int(0, 100);

	for (auto& i : rand.inconsistencies)
		if (!i.should_start && ++i.frequency_counter >= i.frequency)
		{
			if (i.chance <= rand_100)
				i.should_start = true;
			else
				i.frequency_counter -= i.frequency_counter * (i.frequency_counter / i.frequency * 0.85f);
		}		

	for (auto& i2 : rand.inconsistencies2)
		if (!i2.should_start && ++i2.frequency_counter >= i2.frequency)
		{
			if (i2.chance <= rand_100)
				i2.should_start = true;
			else
				i2.frequency_counter -= i2.frequency_counter * (i2.frequency_counter / i2.frequency * 0.85f);
		}

	for (auto& b : rand.boosts)
	{
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
				b_other.frequency_counter -= b_other.frequency_counter * (b_other.frequency_counter / b_other.frequency * 0.85f);
				b.paused = true;
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
			std::cout << "inconsistency\n";
			rand.delay += rand_float(i.min_amount_ms, i.max_amount_ms);
			i.Reset();
			break;
		}
	}

	for (auto& boost : rand.boosts)
	{
		if (boost.start)
		{
			std::cout << "boost\n";
			boost.counter++;
			// boost up
			if (boost.counter <= boost.transition_duration)
			{
				rand.edited_min -= boost.amount_ms;
			}

			// boost down 
			else if (boost.counter >= boost.duration - boost.transition_duration
				&&
				boost.counter <= boost.duration)
			{
				rand.edited_min += boost.amount_ms;
			}

			// reset this boost 
			else if (boost.counter > boost.duration)
			{
				boost.Reset();

				// unpause other boosts
				for (auto& b : rand.boosts)
					b.Reset();
			}

			break; // make sure we dont apply other boosts while boosting 
		}
	}
}

void toadll::CLeftAutoClicker::mouse_down()
{
	randomize_start();

	rand.delay = rand_float(rand.edited_min, rand.edited_max);

	std::cout << rand.delay << ": " << rand.edited_min << ", " << rand.edited_max << " ";

	apply_rand(rand.inconsistencies);

	std::cout << rand.delay << std::endl;
	preciseSleep(rand.delay / 1000.f);
	
	POINT pt{};
	GetCursorPos(&pt);
	PostMessage(GetForegroundWindow(), WM_LBUTTONDOWN, MKF_LEFTBUTTONDOWN, LPARAM((pt.x, pt.y)));

	update_rand_vars();
}

void toadll::CLeftAutoClicker::mouse_up()
{
	rand.delay = rand_float(rand.edited_min, rand.edited_max);

	apply_rand(rand.inconsistencies2);

	preciseSleep(rand.delay / 1000.f);

	POINT pt{};
	GetCursorPos(&pt);
	PostMessage(GetForegroundWindow(), WM_LBUTTONUP, 0, LPARAM((pt.x, pt.y)));

	update_rand_vars();
}

void toadll::CLeftAutoClicker::SetDelays()
{
	rand.min_delay = (1000.f / clicker::cps - 2) / 2;
	rand.max_delay = (1000.f / clicker::cps + 2) / 2;
}

void toadll::CLeftAutoClicker::Update(const std::shared_ptr<c_Entity>& lPlayer)
{
	static bool is_starting_click = false;
	if (!clicker::enabled)
	{
		SLOW_SLEEP(250);
		return;
	}

	if (GetAsyncKeyState(VK_LBUTTON) && !g_is_cursor_shown)
	{
		if (!is_starting_click)
		{
			rand.edited_min = rand.min_delay;
			rand.edited_max = rand.max_delay;
			is_starting_click = true;
		}

		mouse_down();
		mouse_up();
	}
	else
	{
		if (is_starting_click)
		{
			for (auto& i : rand.boosts)
			{
				i.paused = false;
			}
			is_starting_click = false;
		}
		SLOW_SLEEP(10);
	}


}
