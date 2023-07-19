#include "pch.h"
#include "Toad/Toad.h"
#include "clicker_base.h"

namespace toadll
{
	void CClickerBase::update_rand_vars()
	{
		auto& rand = get_rand();

		rand.inconsistency_delay = std::lerp(rand.inconsistency_delay, 0, m_pTick * 1.5f);

		const int rand_100 = RandInt(0, 100);

		for (auto& i : rand.inconsistencies)
			if (!i.start && ++i.frequency_counter >= i.frequency)
			{
				if (i.chance >= rand_100)
					i.start = true;
				else
					i.frequency_counter -= static_cast<float>(i.frequency_counter) * (static_cast<float>(i.frequency_counter) / static_cast<float>(i.frequency) * 0.75f);
			}

		for (auto& i2 : rand.inconsistencies2)
			if (!i2.start && ++i2.frequency_counter >= i2.frequency)
			{
				if (i2.chance <= rand_100)
					i2.start = true;
				else
					i2.frequency_counter -= static_cast<float>(i2.frequency_counter) * (static_cast<float>(i2.frequency_counter) / static_cast<float>(i2.frequency) * 0.75f);
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
					b_other.frequency_counter -= static_cast<float>(b_other.frequency_counter) * (static_cast<float>(b_other.frequency_counter) / static_cast<float>(b_other.frequency) * 0.75f);
					b_other.paused = true;
				}
			}

			b.frequency_counter++;
		}
	}

	void CClickerBase::apply_rand(std::vector<Inconsistency>& inconsistencies)
	{
		auto& rand = get_rand();

		for (auto& i : inconsistencies)
		{
			if (i.start)
			{
				rand.inconsistency_delay = RandFloat(i.min_amount_ms, i.max_amount_ms);
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
}
