#pragma once

namespace toadll
{

class CLeftAutoClicker SET_MODULE_CLASS(CLeftAutoClicker)
{
private:

	struct Inconsistency
	{
		Inconsistency(float min, float max, int chance, int frequency) :
			min_amount_ms(min), max_amount_ms(max), chance(chance), frequency(frequency) {}

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
			frequency_counter = frequency_counter -= frequency_counter * (frequency_counter / frequency * 0.85f);
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
		const vec2 frequency_range = {10, 20};

		// how many clicks it takes to boost up / down
		// amount must be so that transition_duration < duration / 2
		const int transition_duration = 5;  

		// increments every click, used when in boost
		int counter = 0;

		// increments every click, is used to check when we can boost: frequency_counter >= frequency
		int frequency_counter = 0;
	};

	static inline struct Randomization
	{
		float min_delay = 0;
		float max_delay = 0;

		float edited_min = 20;
		float edited_max = 50;

		float delay = 0;

		bool is_start_randomized = false;
		bool start_rand_flag = false;

		// inside the mouse_down function
		std::vector<Inconsistency> inconsistencies = 
		{
			Inconsistency( 30.f, 50.f , 40, 35),
			Inconsistency( 60.f, 80.f , 20, 50),
			Inconsistency( 90.f, 110.f, 10, 60)
		};

		// inside the mouse_up function
		std::vector<Inconsistency> inconsistencies2 =
		{
			Inconsistency( -10.f, 0, 30, 40),
			Inconsistency( -20.f, 0, 10, 50)
		};

		std::vector<Boost> boosts =
		{
			Boost(0.7f, 30, 5, {50,  70}, 0),
			Boost(0.9f, 50, 8, {80, 100}, 1)
		};
		
	} rand;

private:
	static inline void randomize_start();

private:
	/**
	 * @brief
	 * Updates randomization counters and flags. \n
	 * The randomization is based of cps as counters are incremented per click.
	 */
	inline void update_rand_vars();

	/**
	 * @brief
	 * Applies the current randomization variables to the rand.delay. \n
	 * Should be called before using sleep and after setting the default value of delay
	 */
	inline void apply_rand(std::vector<Inconsistency>& inconsistencies);

	inline void mouse_down();
	inline void mouse_up();

public:
	/**
	 * @brief
	 * Updates the min and max delay.
	 * Should always be called when changing the cps 
	 */
	static void SetDelays();
	
public:
	void Update(const std::shared_ptr<c_Entity>& lPlayer) override;
};

}