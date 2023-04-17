#include "pch.h"
#include "Toad/Toad.h"
#include "esp.h"

namespace toadll {

void CEsp::Update(const std::shared_ptr<c_Entity>& lPlayer, float partialTick)
{
	//if (!is_enabled) return;

	static name_pos_t prev_name_map = {};
	name_pos_t name_map = {};
	name_bbox_t name_bbox_map = {};

	static auto ari = p_Minecraft->get_active_render_info();
	auto fov = p_Minecraft->get_fov();

	for (auto playerList = p_Minecraft->get_playerList(); const auto & e : playerList)
	{
		if (env->IsSameObject(lPlayer->obj, e->obj)) continue;

		vec2 screenposition{ 0,0 };
		vec3 realPos = lPlayer->get_position() + ari->get_render_pos();
		vec2 viewangles{ lPlayer->get_rotationYaw(), lPlayer->get_rotationPitch() };

		vec3 ePos = e->get_position();
		float yawDiff = abs(wrap_to_180(-(lPlayer->get_rotationYaw() - get_angles(lPlayer->get_position(), ePos).first)));
		if (yawDiff > 110) continue; // will not render anyway
		
		if (WorldToScreen(realPos, ePos, viewangles, fov, screenposition))
		{
			name_map[e->get_name().c_str()] = screenposition;
		}

		/*vec2 sposmin {}, sposmax{};
		
		if (WorldToScreen(realPos, eBBox.min, viewangles, fov, screenposition))
		{
			sposmin = screenposition;
		}
		if (WorldToScreen(realPos, eBBox.max, viewangles, fov, screenposition))
		{
			sposmax = screenposition;
		}

		name_bbox_map[e->get_name().c_str()] = { sposmin.x, sposmin.y, sposmax.x, sposmax.y };*/
	}

	/*prev_name_map = name_map;

	for (const auto& prevmapit : prev_name_map)
	{
		if (const auto& it2 = std::find_if(name_map.begin(), name_map.end(), [&](const std::pair<const char*, vec2>& f){return strcmp(f.first, prevmapit.first) == 0; }); it2 == name_map.end())
		{
			prev_name_map.erase(it2);
		}
	}

	for (const auto& it : name_map)
	{
		if (auto it2 = std::find_if(prev_name_map.begin(), prev_name_map.end(), [&](const std::pair<const char*, vec2>& f) { return strcmp(it.first, f.first) == 0; }); it2 != player_namepos_map.end())
		{
			it2->second = it.second;
		}
	}*/

	player_bboxpos_map = name_bbox_map;
	player_namepos_map = name_map;

}

const CEsp::name_pos_t& CEsp::get_playernames_map() const
{
	return this->player_namepos_map;
}

const CEsp::name_bbox_t& CEsp::get_bounding_box_map() const
{
	return this->player_bboxpos_map;
}
}
