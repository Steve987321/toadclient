#pragma once

namespace toadll
{

#ifdef TOAD_LOADER
	inline int rand_int(int min, int max)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dis(min, max);
		return dis(gen);
	}
#endif

	struct Inconsistency
	{
		Inconsistency(float min, float max, int chance, int frequency) :
			min_amount_ms(min), max_amount_ms(max), frequency(frequency), chance(chance)
		{
			Reset();
		}

		void Reset()
		{
			start = false;
			frequency_counter = rand_int(frequency / 5, frequency / 4);
		}

		/// True when this inconsistency should be applied to the rand
		bool start = false;

		/// picks a random number between..
		float min_amount_ms = 0.0f, max_amount_ms = 1.0f;

		/// How frequently this inconsistency should occur
		/// this frequency is based of clicks.
		///
		/// A higher value will occur less and a lower value will occur more
		int frequency = 10;

		/// chance in %.
		///
		/// Depends on the frequency: chance is used when counter > frequency 
		int chance = 50;

		/// increments every click
		int frequency_counter = 0;
	};

	struct Boost
	{
		Boost(float amount, float dur, float transition_dur, float freqmin, float freqmax, int id) :
			amount_ms(amount), duration(dur), transition_duration(transition_dur), frequency(rand_int(freqmin, freqmax)), freq_min(freqmin), freq_max(freqmax), id(id) {}

		void Reset()
		{
			start = false;
			paused = false;
			counter = 0;
			frequency_counter = frequency_counter -= static_cast<float>(frequency_counter) * (static_cast<float>(frequency_counter) / static_cast<float>(frequency) * 0.75f);
			frequency = rand_int(freq_min, freq_max);
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
		const int freq_min = 10, freq_max = 20;

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