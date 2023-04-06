#include "pch.h"
#include "Toad/Toad.h"
#include "esp.h"

namespace toadll::esp
{
	void UpdatePlayerMap(const std::shared_ptr<c_Entity>& lPlayer)
	{
		//if (!EntityEsp::enabled) return;

		static auto ari = p_Minecraft->get_active_render_info();

		//playerListMap[]
		//renderNames.resize(playerList.size(), { {-1, -1}, nullptr });

		for (auto playerList = p_Minecraft->get_playerList(); const auto& e : playerList)
		{
			if (env->IsSameObject(lPlayer->obj, e->obj)) continue;

			vec2 screenposition{ 0,0 };
			vec3 realPos = lPlayer->get_position() + ari->get_render_pos();
			vec2 viewangles{ lPlayer->get_rotationYaw(), lPlayer->get_rotationPitch() };

			vec3 ePos = e->get_position();
			float yawDiff = abs(wrap_to_180(-(lPlayer->get_rotationYaw() - get_angles(lPlayer->get_position(), ePos).first)));
			//p_Log->LogToConsole(std::to_string(yawDiff));

			if (yawDiff > 110) continue; // will not render anyway

			if (WorldToScreen(realPos, ePos, viewangles, p_Minecraft->get_fov(), screenposition))
			{
				if (auto it = playerListMap.find(e->get_name().c_str()); it != playerListMap.end())
					it->second = screenposition;
				else
					playerListMap.insert({e->get_name().c_str(), screenposition});
			}
		}
		//std::cout << playerListMap.size() << std::endl;
	}

}
