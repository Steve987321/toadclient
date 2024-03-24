#pragma once

///
/// This should act as a clicker without clicking 
///
class VisualizeClicker
{
public:
	VisualizeClicker();
	~VisualizeClicker();

public:
	void Start();
	void Stop();

public:
	int GetCPS() const;
	toadll::Randomization GetRand();

public:
	void SetRand(const toadll::Randomization& rand);

public:
	float d_time = 0;

private:
	void clicking_thread();

private:
	// same as clicker
	void click_down();
	void click_up();

	void apply_rand(std::vector<toadll::Inconsistency>& inconsistencies);
	void update_rand();

private:
	toadll::Timer m_rand_delay_timer;

	std::queue<toadll::Timer> m_click_queue;

	std::thread m_clicking_thread;
	std::atomic_bool m_thread_running = false;

private:
	// same as left clicker 
	toadll::Randomization m_rand = toad::left_clicker::rand;
};

