#pragma once

#include "Types.h"

namespace toadll
{
	class c_AimAssist
	{
	private:
		std::thread m_thread;
		std::atomic_bool m_thread_running = false;

	private:
		void thread() const;

	public:
		void start_thread();
		void stop_thread();

		bool is_running() const;
	};

	inline std::unique_ptr<c_AimAssist> p_AimAssist = nullptr;
}
