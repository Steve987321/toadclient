#pragma once

namespace toadll
{

class CTimer
{
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start;

public:
	CTimer()
	{
		Start();
	}

public:
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