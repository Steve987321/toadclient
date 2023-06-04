#include "pch.h"
#include "Toad/Toad.h"
#include "esp.h"

namespace toadll {

void CEsp::OnTick(const std::shared_ptr<c_Entity>& lPlayer)
{
	auto playerList = p_Minecraft->get_playerList();
	auto ari = p_Minecraft->get_active_render_info();

	auto renderpos = ari->get_render_pos();
	ari->get_modelview(modelview);
	ari->get_projection(projection);

	std::vector<std::shared_ptr<EntityVisual>> evlist {};

	for (const auto& entity : playerList)
	{
		if (env->IsSameObject(lPlayer->obj, entity->obj))
			continue;

		auto bbox = entity->get_BBox();
		auto epos = entity->get_position();

	/*	auto eposlasttick = entity->get_lasttickposition();

		bbox.min.x = bbox.min.x + (-epos.x) + (eposlasttick.x + (epos.x - eposlasttick.x) * partialTick) + (-renderpos.x);
		bbox.min.y = bbox.min.y + (-epos.y) + (eposlasttick.y + (epos.y - eposlasttick.y) * partialTick) + (-renderpos.y);
		bbox.min.z = bbox.min.z + (-epos.z) + (eposlasttick.z + (epos.z - eposlasttick.z) * partialTick) + (-renderpos.z);
		bbox.max.x = bbox.max.x + (-epos.x) + (eposlasttick.x + (epos.x - eposlasttick.x) * partialTick) + (-renderpos.x);
		bbox.max.y = bbox.max.y + (-epos.y) + (eposlasttick.y + (epos.y - eposlasttick.y) * partialTick) + (-renderpos.y);
		bbox.max.z = bbox.max.z + (-epos.z) + (eposlasttick.z + (epos.z - eposlasttick.z) * partialTick) + (-renderpos.z);*/

		bbox.min.x = epos.x - bbox.min.x - renderpos.x;
		bbox.min.y = epos.y - bbox.min.y - renderpos.y;
		bbox.min.z = epos.z - bbox.min.z - renderpos.z;
		bbox.max.x = epos.x - bbox.max.x - renderpos.x;
		bbox.max.y = epos.y - bbox.max.y - renderpos.y;
		bbox.max.z = epos.z - bbox.max.z - renderpos.z;

		vec3 boxVertices[8]
		{
			/*{bbox.min.x - 0.1f, bbox.min.y - 0.1f, bbox.min.z - 0.1f},
			{bbox.min.x - 0.1f, bbox.max.y + 0.1f, bbox.min.z - 0.1f},
			{bbox.max.x + 0.1f, bbox.max.y + 0.1f, bbox.min.z - 0.1f},
			{bbox.max.x + 0.1f, bbox.min.y - 0.1f, bbox.min.z - 0.1f},
			{bbox.max.x + 0.1f, bbox.max.y + 0.1f, bbox.max.z + 0.1f},
			{bbox.min.x - 0.1f, bbox.max.y + 0.1f, bbox.max.z + 0.1f},
			{bbox.min.x - 0.1f, bbox.min.y - 0.1f, bbox.max.z + 0.1f},
			{bbox.max.x + 0.1f, bbox.min.y - 0.1f, bbox.max.z + 0.1f},*/
			{bbox.min.x, bbox.min.y, bbox.min.z},
			{bbox.min.x, bbox.max.y, bbox.min.z},
			{bbox.max.x, bbox.max.y, bbox.min.z},
			{bbox.max.x, bbox.min.y, bbox.min.z},
			{bbox.min.x, bbox.min.y, bbox.max.z},
			{bbox.min.x, bbox.max.y, bbox.max.z},
			{bbox.max.x, bbox.max.y, bbox.max.z},
			{bbox.max.x, bbox.min.y, bbox.max.z},
		};

		vec4 screenbbox = {FLT_MAX, FLT_MAX, -1.f, -1.f};

		for (const auto& boxVertice : boxVertices)
		{
			vec2 screenPos;
			if (WorldToScreen(
				vec3(boxVertice.x, boxVertice.y, boxVertice.z),
				screenPos,
				modelview,
				projection
			))
			{
				screenbbox.x = std::min(screenPos.x, screenbbox.x);
				screenbbox.y = std::min(screenPos.y, screenbbox.y);
				screenbbox.z = std::max(screenPos.x, screenbbox.z);
				screenbbox.w = std::max(screenPos.y, screenbbox.w);
			}
		}

		evlist.emplace_back(std::make_unique<EntityVisual>(
			entity->get_name().c_str(),
			screenbbox
			));
	}

	entityVisualList = evlist;
}

void CEsp::OnRender(ImDrawList* draw)
{
	const auto DrawBox = [](const ImVec2& start, const ImVec2& end, const ImU32& color)
	{
		ImGui::GetBackgroundDrawList()->AddRect(start, end, ImColor(0, 0, 0, 255), 0, 0, 4.f);
		ImGui::GetBackgroundDrawList()->AddRect(start, end, color, 0, 0, 1.f);
	};

	for (const auto& e : entityVisualList)
	{
		auto& bbox = e->bounding_box2d;

		if (bbox.x <= 0 || bbox.y <= 0 || bbox.z >= screen_width || bbox.w >= screen_height)
			continue;

		DrawBox(ImVec2(bbox.x, bbox.y), ImVec2(bbox.z, bbox.w), IM_COL32_WHITE);
	}
}

}
