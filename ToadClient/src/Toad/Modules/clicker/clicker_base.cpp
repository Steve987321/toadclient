#include "pch.h"
#include "Toad/toadll.h"
#include "clicker_base.h"

namespace toadll
{
	void CClickerBase::update_rand_vars()
	{
		Randomization& rand = get_rand();

		rand.inconsistency_delay = std::lerp(rand.inconsistency_delay, 0.f, m_pTick * 1.5f);

		const int rand_100 = rand_int(0, 100);

		for (Inconsistency& i : rand.inconsistencies)
			if (!i.start && ++i.frequency_counter >= i.frequency)
			{
				if (i.chance >= rand_100)
					i.start = true;
				else
					i.frequency_counter -= i.frequency_counter * (int)((float)i.frequency_counter / (float)i.frequency * 0.75f);
			}

		for (Inconsistency& i2 : rand.inconsistencies2)
			if (!i2.start && ++i2.frequency_counter >= i2.frequency)
			{
				if (i2.chance >= rand_100)
					i2.start = true;
				else
					i2.frequency_counter -= i2.frequency_counter * (int)((float)i2.frequency_counter / (float)i2.frequency * 0.75f);
			}

		for (Boost& b : rand.boosts)
		{
			if (b.paused)
				continue;

			if (!b.start && ++b.frequency_counter >= b.frequency)
			{
				b.start = true;

				for (Boost& b_other : rand.boosts)
				{
					if (b_other.id == b.id)
						continue;

					// because we are boosting the cps we want to
					// make boosting less frequent and pause frequency counters for other boosters
					b_other.frequency_counter += 25;
					b_other.paused = true;
				}
			}

			b.frequency_counter++;
		}
	}

	void CClickerBase::apply_rand(std::vector<Inconsistency>& inconsistencies)
	{
		Randomization& rand = get_rand();

		for (Inconsistency& i : inconsistencies)
		{
			if (i.start)
			{
				rand.inconsistency_delay = rand_float(i.min_amount_ms, i.max_amount_ms);
				i.Reset();
				break;
			}
		}

		for (Boost& boost : rand.boosts)
		{
			if (boost.start)
			{
				boost.counter++;

				// boost up
				if (boost.counter <= boost.transition_duration)
				{
					rand.edited_min -= boost.amount_ms / boost.transition_duration;
					rand.edited_max -= boost.amount_ms / boost.transition_duration;
					LOGDEBUG("boost up rand.edited_* -= {} edited_min = {}", boost.amount_ms / boost.transition_duration, rand.edited_min);
				}

				// boost down 
				else if (boost.counter > boost.duration - boost.transition_duration
					&&
					boost.counter <= boost.duration)
				{
					rand.edited_min += boost.amount_ms / boost.transition_duration;
					rand.edited_max += boost.amount_ms / boost.transition_duration;
					LOGDEBUG("boost up rand.edited_* += {} edited_min = {}", boost.amount_ms / boost.transition_duration, rand.edited_min);
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
}
