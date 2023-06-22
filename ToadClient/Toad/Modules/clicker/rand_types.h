#pragma once

namespace toadll
{
	struct Inconsistency
	{
		Inconsistency(float min, float max, int chance, int frequency) :
			min_amount_ms(min), max_amount_ms(max), chance(chance), frequency(frequency)
		{
			Reset();
		}

		void Reset()
		{
			should_start = false;
			frequency_counter = rand_int(frequency / 5, frequency / 4);
		}

		bool should_start = false;

		// picks a random number inbetween
		const float min_amount_ms = 0.0f, max_amount_ms = 1.0f;

		// how frequently this inconsistency should occur
		// this is based of clicks, a higher value will occur less and a lower value will occur more.
		const int frequency = 10;

		// chance in %, depends on the frequency: chance is used when counter > frequency. 
		const int chance = 50;

		// increments every click
		int frequency_counter = 0;
	};

	struct Boost
	{
		Boost(float amount, float dur, float transition_dur, vec2 freq, int id) :
			amount_ms(amount), duration(dur), transition_duration(transition_dur), frequency(rand_int(freq.x, freq.y)), frequency_range(freq), id(id) {}

		void Reset()
		{
			start = false;
			paused = false;
			counter = 0;
			frequency_counter = frequency_counter -= static_cast<float>(frequency_counter) * (static_cast<float>(frequency_counter) / static_cast<float>(frequency) * 0.75f);
			frequency = rand_int(frequency_range.x, frequency_range.y);
		}

		bool start = false;

		// pause the frequency counter
		bool paused = false;

		// when comparing 
		int id = 0;

		// gets subtracted from Randomization.min_delay when in transition
		// The full boost amount will be amount * transition_duration
		const float amount_ms = 0.2f;

		// full duration of the boost in clicks
		const int duration = 30;

		// there is no chance variable which makes boosts more consistent
		// this frequency is randomized with frequency_range
		int frequency = 0;

		// frequency random range
		const vec2 frequency_range = { 10, 20 };

		// how many clicks it takes to boost up / down
		// amount must be so that transition_duration < duration / 2
		const int transition_duration = 5;

		// increments every click, used when in boost
		int counter = 0;

		// increments every click, is used to check when we can boost: frequency_counter >= frequency
		int frequency_counter = 0;
	};

	struct Randomization
	{
		Randomization(
			float min_delay, float max_delay, 
			float edited_min, float edited_max, 
			float delay,
			bool start_rand_flag,
			float inconsistency_delay,
			std::vector<Inconsistency> inconsistencies,
			std::vector<Inconsistency> inconsistencies2,
			std::vector<Boost> boosts) 
		{
			this->min_delay = min_delay;
			this->max_delay = max_delay;

			this->edited_min = edited_min;
			this->edited_max = edited_max;

			this->delay = delay;

			this->start_rand_flag = start_rand_flag;
			 
			this->inconsistency_delay = inconsistency_delay;

			this->inconsistencies = std::move(inconsistencies);
			this->inconsistencies2 = std::move(inconsistencies2);

			this->boosts = std::move(boosts);
		}

		float min_delay = 0;
		float max_delay = 0;

		float edited_min = 20;
		float edited_max = 50;

		float delay = 0;

		bool start_rand_flag = false;

		float inconsistency_delay = 0;

		// inside the mouse_down function
		std::vector<Inconsistency> inconsistencies =
		{
		};

		// inside the mouse_up function
		std::vector<Inconsistency> inconsistencies2 =
		{
		};

		std::vector<Boost> boosts =
		{
		};

	};

}