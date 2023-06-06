#pragma once

#include "rand_types.h"

namespace toadll
{

class CLeftAutoClicker SET_MODULE_CLASS(CLeftAutoClicker)
{
private:
	static inline struct Randomization
	{
		float min_delay = 0;
		float max_delay = 0;

		float edited_min = 20;
		float edited_max = 50;

		float delay = 0;

		bool is_start_randomized = false;
		bool start_rand_flag = false;

		float inconsistency_delay = 0;

		// inside the mouse_down function
		std::vector<Inconsistency> inconsistencies = 
		{
			Inconsistency( 10.f, 30.f , 70, 35),
			Inconsistency( 20.f, 40.f , 60, 50),
			Inconsistency( 40.f, 60.f, 50, 150),

			Inconsistency(-10.f, 0    , 50, 40),
		};

		// inside the mouse_up function
		std::vector<Inconsistency> inconsistencies2 =
		{
			Inconsistency(30.f, 50.f , 70, 50),
			Inconsistency(60.f, 80.f , 50, 100),

			Inconsistency( -10.f, 0, 60, 50),
			Inconsistency( -15.f, 0, 40, 60)
		};

		std::vector<Boost> boosts =
		{
			Boost(3.5f, 30, 3, {100,  150}, 0),
			Boost(2.8f, 50, 5, {100,  150}, 1),
			Boost(1.8f, 120, 8, {160, 200}, 2),
			Boost(2.7f, 150, 5, {160, 200}, 3)
		};
		
	} rand;

private:
	float m_delay_compensation = 0;
	std::chrono::high_resolution_clock::time_point m_start, m_end;

	float m_pTick = 0;

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

private:
	inline std::string get_mouse_over_type() const;

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
