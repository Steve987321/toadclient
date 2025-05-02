#pragma once

#include "Toad/timer.h"
#include "Toad/Modules/clicker/rand_types.h"
#include "Toad/toad_defs.h"
#include "Toad/config.h"

#include <queue>
#include <atomic>

///
/// This should act as a clicker without clicking 
///
class TOAD_API VisualizeClicker
{
public:
	VisualizeClicker();
	~VisualizeClicker();

public:
	float d_time = 0;

	void Start();
	void Stop();

	bool IsStarted() const;

	float GetCPS() const;
	toad::Randomization GetRand();

	void SetRand(const toad::Randomization& rand);

	void SetClickCallback(const std::function<void()>& f);

private:
	void clicking_thread();

	// same as clicker
	void click_down();
	void click_up();

	void apply_rand(std::vector<toad::Inconsistency>& inconsistencies);
	void update_rand();

private:
	toad::Timer m_rand_delay_timer;

	std::queue<toad::Timer> m_click_queue;

	std::thread m_clicking_thread;
	std::atomic_bool m_thread_running = false;

	// same as left clicker 
	toad::Randomization m_rand = toad::Config::Get().lc_rand;

	std::function<void()> m_callback;
};

