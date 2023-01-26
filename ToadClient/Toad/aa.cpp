#include "pch.h"
#include "aa.h"
#include "Toad/Toad.h"

void toadll::c_AimAssist::thread() const
{
	while (m_thread_running)
	{
		if (GetAsyncKeyState(aa::key))
		{			
			std::vector <std::pair<float, std::shared_ptr<c_Entity>>> distances = {};
			auto lPlayer = p_Minecraft->get_localplayer();

			for (const auto& player : p_Minecraft->get_playerList())
			{
				if (player->obj == lPlayer->obj) continue;
				std::cout << "plaeyr get\n";
				distances.emplace_back(lPlayer->get_position().dist(player->get_position()), player);
			}

			if (distances.size() < 2) return;
			auto t = std::min_element(distances.begin(), distances.end());
			auto target = t->second;

			auto [yaw, pitch] = get_angles(lPlayer->get_position(), target->get_position());

			float difference = wrap_to_180(-(lPlayer->get_rotationYaw() - yaw));
			float difference2 = wrap_to_180(-(lPlayer->get_rotationPitch() - pitch));

			log_Debug("yaw diff: ", difference);
			log_Debug("pitch diff: ", difference2);

			//lPlayer->set_rotationYaw(lerp(lplayer_yaw, lplayer_yaw + difference, lplayer_yaw / (lplayer_yaw + difference)));
			//lPlayer->set_rotationPitch(lerp(lplayer_pitch, lplayer_pitch + difference2, lplayer_yaw / (lplayer_pitch + difference2)));
	
		}
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
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