#include "pch.h"
#include "aa.h"
#include "Toad/Toad.h"

void toadll::c_AimAssist::thread()
{
	while (m_thread_running)
	{
		if (GetAsyncKeyState(aa::key))
		{
			for (const auto & player : p_Minecraft->get_playerList())
			{
				
			}
		}
	}
}

void toadll::c_AimAssist::start_thread()
{
	m_thread_running = true;
	m_thread = std::thread(&c_AimAssist::thread, this);
}

void toadll::c_AimAssist::stop_thread()
{
	m_thread_running = false;
	if (m_thread.joinable()) m_thread.join();
}

bool toadll::c_AimAssist::is_running() const
{
	return m_thread_running;
}