#ifdef TOAD_LOADER
#include "global_settings.h"

#include <thread>
#include <queue>

#include "VisualizeClicker.h"

#include <iostream>
#endif

VisualizeClicker::VisualizeClicker()
{
}

VisualizeClicker::~VisualizeClicker()
{
	Stop();
}

void VisualizeClicker::Start()
{
	m_thread_running = true;
	m_clicking_thread = std::thread(&VisualizeClicker::clicking_thread, this);
} 

void VisualizeClicker::Stop()
{
	m_thread_running = false;
	if (m_clicking_thread.joinable()) m_clicking_thread.join();
}

int VisualizeClicker::GetCPS() const
{
	return m_click_queue.size();
}

toadll::Randomization VisualizeClicker::GetRand()
{
	return m_rand;
}

void VisualizeClicker::SetRand(const toadll::Randomization& rand)
{
	bool restart = false;
	if (m_thread_running)
	{
		restart = true;
		Stop();
	}
	m_rand = rand;
	if (restart)
		Start();
}

void VisualizeClicker::clicking_thread()
{
	while (m_thread_running)
	{
		// m_delayTimer gets resetted in the click functions 
		click_down();
		while (m_rand_delay_timer.Elapsed<>() < m_rand.delay + m_rand.inconsistency_delay)
		{
			if (!m_click_queue.empty())
				if (m_click_queue.front().Elapsed<>() >= 1000.f)
				{
					m_click_queue.pop();
				}
		}
		click_up();
		while (m_rand_delay_timer.Elapsed<>() < m_rand.delay + m_rand.inconsistency_delay)
		{
			if (!m_click_queue.empty())
				if (m_click_queue.front().Elapsed<>() >= 1000.f)
				{
					m_click_queue.pop();
				}
		}
	}
}

void VisualizeClicker::click_down()
{
	m_rand.delay = toadll::rand_float(m_rand.edited_min, m_rand.edited_max);

	apply_rand(m_rand.inconsistencies);

	m_rand_delay_timer.Start(); // where the sleep should be in the actual clicker

	m_click_queue.emplace();

	update_rand();
}

void VisualizeClicker::click_up()
{
	m_rand.delay = toadll::rand_float(m_rand.edited_min, m_rand.edited_max);

	apply_rand(m_rand.inconsistencies2);

	m_rand_delay_timer.Start(); // where the sleep should be in the actual clicker

	// don't add, only for click_down
	//m_trackCpsQueue.emplace();

	update_rand();
}

void VisualizeClicker::apply_rand(std::vector<toadll::Inconsistency>& inconsistencies)
{
	for (auto& i : inconsistencies)
	{
		if (i.start)
		{
			m_rand.inconsistency_delay = toadll::rand_float(i.min_amount_ms, i.max_amount_ms);
			i.Reset();
			break;
		}
	}

	for (auto& boost : m_rand.boosts)
	{
		if (boost.start)
		{
			boost.counter++;
			// boost up
			if (boost.counter <= boost.transition_duration)
			{
				//log_Debug("id: %d boosting up (%f, %f)", boost.id, rand.edited_min, rand.edited_max);
				m_rand.edited_min -= boost.amount_ms;
				m_rand.edited_max -= boost.amount_ms / 2.0f;
			}

			// boost down 
			else if (boost.counter > boost.duration - boost.transition_duration
				&&
				boost.counter <= boost.duration)
			{
				//log_Debug("id: %d boosting down (%f, %f)", boost.id, rand.edited_min, rand.edited_max);
				m_rand.edited_min += boost.amount_ms;
				m_rand.edited_max += boost.amount_ms / 2.0f;
			}

			// reset this boost 
			else if (boost.counter > boost.duration)
			{
				boost.Reset();
				// unpause other boosts
				for (auto& b : m_rand.boosts)
					b.paused = false;
			}

			break; // make sure we dont apply other boosts while boosting 
		}
	}
}

void VisualizeClicker::update_rand()
{
	static toadll::Timer timer;
	timer.Start();
	m_rand.inconsistency_delay = std::lerp(m_rand.inconsistency_delay, 0, 0.3f);

	const int rand_100 = toadll::rand_int(0, 100);

	for (auto& i : m_rand.inconsistencies)
		if (!i.start && ++i.frequency_counter >= i.frequency)
		{
			if (i.chance >= rand_100)
				i.start = true;
			else
				i.frequency_counter -= static_cast<float>(i.frequency_counter) * (static_cast<float>(i.frequency_counter) / static_cast<float>(i.frequency) * 0.75f);
		}

	for (auto& i2 : m_rand.inconsistencies2)
		if (!i2.start && ++i2.frequency_counter >= i2.frequency)
		{
			if (i2.chance <= rand_100)
				i2.start = true;
			else
				i2.frequency_counter -= static_cast<float>(i2.frequency_counter) * (static_cast<float>(i2.frequency_counter) / static_cast<float>(i2.frequency) * 0.75f);
		}

	for (auto& b : m_rand.boosts)
	{
		if (b.paused)
			continue;

		if (!b.start && ++b.frequency_counter >= b.frequency)
		{
			b.start = true;

			// because we are boosting the cps we want to
			// make boosting less frequent and pause frequency counters for other boosters
			for (auto& b_other : m_rand.boosts)
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
