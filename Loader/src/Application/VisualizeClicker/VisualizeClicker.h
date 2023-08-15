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
	float dTime = 0;

private:
	void clicking_thread();

private:
	// ~same as clicker
	void click_down();
	void click_up();

	void apply_rand(std::vector<toadll::Inconsistency>& inconsistencies);
	void update_rand();

private:
	toadll::Timer m_randDelayTimer;

	std::queue<toadll::Timer> m_trackCpsQueue;

private:
	std::thread m_clicking_thread;
	std::atomic_bool m_isThreadRunning = false;

private:
	// ~same as clicker 
	toadll::Randomization m_rand = toad::left_clicker::rand;
};

