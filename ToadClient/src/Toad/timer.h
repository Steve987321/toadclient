#pragma once

namespace toadll
{

///
/// Simple timer class
///
class Timer
{
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start;

public:
	Timer()
	{
		Start();
	}

public:
	/// Starts or Resets the begin point of the timer
	void Start()
	{
		m_start = std::chrono::high_resolution_clock::now();
	}

	/// Returns the time interval between m_start and now.
	///
	/// DurationT specifies the time metric 
	template<typename DurationT = std::chrono::milliseconds>
	_NODISCARD float Elapsed() const
	{
		return (float)std::chrono::duration_cast<DurationT>(std::chrono::high_resolution_clock::now() - m_start).count();
	}
};

}