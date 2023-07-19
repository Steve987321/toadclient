#pragma once

namespace toadll
{

class Timer
{
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start;

public:
	Timer()
	{
		Start();
	}

public:
	// Starts or Resets the begin point of the timer
	void Start()
	{
		start = std::chrono::high_resolution_clock::now();
	}

	template<typename DurationT = std::chrono::milliseconds>
	_NODISCARD float Elapsed() const
	{
		return std::chrono::duration_cast<DurationT>(std::chrono::high_resolution_clock::now() - start).count();
	}
};

}