#include "pch.h"
#include "Toad/Toad.h"
#include "esp.h"

#include "draw_helpers.h"

using namespace toad;

namespace toadll {

void CEsp::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	if (!esp::enabled || !CVarsUpdater::IsVerified)
	{
		SLEEP(250);
		return;
	}

	renderPos = MC->getActiveRenderInfo()->get_render_pos();
	playerPos = lPlayer->Pos;

	// Update our bounding boxes list
	m_bboxes = GetBBoxes();
}

void CEsp::OnRender()
{
	if (!esp::enabled || !CVarsUpdater::IsVerified)
	{
		//std::lock_guard lock(m_bboxesMutex);
		m_bboxes.clear();
		return;
	}

	if (m_bboxes.empty())
		return;

	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(CVarsUpdater::Projection.data());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(CVarsUpdater::ModelView.data());

	glPushMatrix();
	glEnable(GL_LINE_SMOOTH);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDepthMask(false);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glLineWidth(1.f);

	//m_bboxesMutex.lock();
	for (const auto& e : m_bboxes)
	{
		switch (esp::esp_mode)
		{
		case ESP_MODE::BOX3D:
			draw3d_bbox_fill(
				e.bb,
				{ esp::fill_col[0], esp::fill_col[1], esp::fill_col[2], esp::fill_col[3] }
			);
			draw3d_bbox_lines(
				e.bb,
				{ esp::line_col[0], esp::line_col[1], esp::line_col[2], esp::line_col[3] }
			);
			break;

		case ESP_MODE::BOX2D_DYNAMIC: // uses imgui 
			break;

		case ESP_MODE::BOX2D_STATIC:
			draw2d_bbox(
				e.bb,
				{ esp::fill_col[0], esp::fill_col[1], esp::fill_col[2], esp::fill_col[3] },
				{ esp::line_col[0], esp::line_col[1], esp::line_col[2], esp::line_col[3] }
			);
			break;

		default: 
			break ;
		}
	}
	//m_bboxesMutex.unlock();

	glDisable(GL_BLEND);
	glDepthMask(true);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LINE_SMOOTH);
	glPopMatrix();

	glPopMatrix();
}

void CEsp::OnImGuiRender(ImDrawList* draw)
{
	if (!esp::enabled || !CVarsUpdater::IsVerified)
	{
		return;
	}

	if (esp::esp_mode == ESP_MODE::BOX2D_DYNAMIC)
	{
		for (const auto& [bb, pos, name, hurttime] : m_bboxes)
		{
			if (esp::show_name || esp::show_distance)
			{
				// get the center top of player 
				auto posAddedY = (bb.min + bb.max) * 0.5f;
				posAddedY.y += 2.f; 

				auto screenpos = world_to_screen(posAddedY, renderPos);

				if ((int)screenpos.x * 10 != -10 && (int)screenpos.y * 10 != -10)
				{
					if (esp::show_name)
					{
						auto textSize = ImGui::CalcTextSize(name.c_str());
						draw->AddText({ screenpos.x - textSize.x / 2, screenpos.y }, IM_COL32_WHITE, name.c_str());
					}
					if (esp::show_distance)
					{
						auto distStr = std::to_string(playerPos.dist(pos)).substr(0, 3);
						auto textSize = ImGui::CalcTextSize(distStr.c_str());
						draw->AddText({ screenpos.x - textSize.x / 2, screenpos.y + 10 }, IM_COL32_WHITE, distStr.c_str());
					}
				}				
			}

			// get vertices from bounding box 
			auto vertices = GetBBoxVertices(bb.min, bb.max);

			std::vector<Vec2> verticesScreenPos = {};

			// world vertices pos to screen pos
			for (const auto& v : vertices)
			{
				auto screen = world_to_screen(v, renderPos);
				verticesScreenPos.emplace_back(screen);
			}

			// 'dynamic'
			auto minX = std::ranges::min_element(verticesScreenPos, [](const Vec2& a, const Vec2& b) { return a.x < b.x; })->x;
			auto minY = std::ranges::min_element(verticesScreenPos, [](const Vec2& a, const Vec2& b) { return a.y < b.y; })->y;
			auto maxX = std::ranges::max_element(verticesScreenPos, [](const Vec2& a, const Vec2& b) { return a.x < b.x; })->x;
			auto maxY = std::ranges::max_element(verticesScreenPos, [](const Vec2& a, const Vec2& b) { return a.y < b.y; })->y;

			// convert color config
			auto line_col = ImGui::ColorConvertFloat4ToU32({ esp::line_col[0], esp::line_col[1], esp::line_col[2], esp::line_col[3]});
			auto fill_col = ImGui::ColorConvertFloat4ToU32({ esp::fill_col[0], esp::fill_col[1], esp::fill_col[2], esp::fill_col[3]});

			// skip invalid worldtoscreen's 
			if ((int)minX * 10 == -10 && maxX > g_screen_width) continue;
			if ((int)minY * 10 == -10 && maxY > g_screen_height) continue;

			draw->AddRectFilled({ minX, minY }, { maxX, maxY }, fill_col);
			draw->AddRect({ minX - esp::line_width, minY - esp::line_width }, { maxX + esp::line_width, maxY + esp::line_width }, line_col);
		}
	}
	else
	{
		if (esp::show_name || esp::show_distance)
		{
			for (const auto& [bb, pos, name, hurttime] : m_bboxes)
			{
				// get the center and top of player 
				auto posAddedY = (bb.min + bb.max) * 0.5f;
				posAddedY.y += 2.f;

				auto screenpos = world_to_screen(posAddedY, renderPos);
				auto text_col = ImGui::ColorConvertFloat4ToU32({ esp::text_col[0], esp::text_col[1], esp::text_col[2], esp::text_col[3] });

				if ((int)screenpos.x * 10 != -10 && (int)screenpos.y * 10 != -10)
				{
					if (esp::show_name)
					{
						auto textSize = ImGui::CalcTextSize(name.c_str());
						auto posX = screenpos.x - textSize.x / 2;

						if (esp::text_shadow)
						{
							draw->AddText({ posX - 1, screenpos.y - 1 }, IM_COL32_BLACK, name.c_str());
							draw->AddText({ posX + 1, screenpos.y + 1 }, IM_COL32_BLACK, name.c_str());
						}
						draw->AddText({ posX, screenpos.y }, text_col, name.c_str());
					}
					if (esp::show_distance)
					{
						auto distStr = std::to_string(playerPos.dist(pos)).substr(0, 3);
						auto textSize = ImGui::CalcTextSize(distStr.c_str());
						auto posX = screenpos.x - textSize.x / 2;

						if (esp::text_shadow)
						{
							draw->AddText({ posX - 1, screenpos.y + 10 - 1}, IM_COL32_BLACK, distStr.c_str());
							draw->AddText({ posX + 1, screenpos.y + 10 + 1}, IM_COL32_BLACK, distStr.c_str());
						}
						draw->AddText({ posX, screenpos.y + 10 }, text_col, distStr.c_str());
					}
				}
			}
		}
	}

	/*Vec2 screen{};
	Vec3 WorldPos = Vec3(-561, 5, -38);
	screen = world_to_screen(WorldPos - debuggingvector3);
	draw->AddCircle({ screen.x, screen.y }, 10, IM_COL32_WHITE);*/
}

std::vector<CEsp::VisualEntity> CEsp::GetBBoxes()
{
	std::vector<VisualEntity> res = {};

	for (const auto& entity : GetPlayerList())
	{
		if (entity.Invis)
			continue;

		// local player 
		auto lPlayer = MC->getLocalPlayer();
		auto playerlasttickpos = lPlayer->getLastTickPosition();
		auto currPos = lPlayer->getPosition();

		auto lpos = playerlasttickpos + (currPos - playerlasttickpos) * CVarsUpdater::RenderPartialTick;
		auto lasttickpos = entity.LastTickPos;
		auto pos = entity.Pos;
		BBox b_box = { {}, {} };
		b_box.min.x = pos.x - 0.3f - lpos.x + -pos.x + lasttickpos.x + (pos.x - lasttickpos.x) * CVarsUpdater::RenderPartialTick;
		b_box.min.y = pos.y - lpos.y + -pos.y + lasttickpos.y + (pos.y - lasttickpos.y) * CVarsUpdater::RenderPartialTick;
		b_box.min.z = pos.z - 0.3f - lpos.z + -pos.z + lasttickpos.z + (pos.z - lasttickpos.z) * CVarsUpdater::RenderPartialTick;
		b_box.max.x = pos.x + 0.3f - lpos.x + -pos.x + lasttickpos.x + (pos.x - lasttickpos.x) * CVarsUpdater::RenderPartialTick;
		b_box.max.y = pos.y + 1.8f - lpos.y + -pos.y + lasttickpos.y + (pos.y - lasttickpos.y) * CVarsUpdater::RenderPartialTick;
		b_box.max.z = pos.z + 0.3f - lpos.z + -pos.z + lasttickpos.z + (pos.z - lasttickpos.z) * CVarsUpdater::RenderPartialTick;

		//entityList.emplace_back(entity->Name, bbox{{-b_box.min.x, -b_box.min.y, -b_box.min.z}, {-b_box.max.x, -b_box.max.y, -b_box.max.z}});
		res.emplace_back(b_box, pos, entity.Name, entity.HurtTime);

		/*visual_entity.name = entity->Name;
		entity_list.push_back(visual_entity);*/
	}

	return res;
}

std::vector<Vec3> CEsp::GetBBoxVertices(const Vec3& min, const Vec3& max)
{
	std::vector<Vec3> res;

	// front face
	res.emplace_back(min.x, min.y, min.z);
	res.emplace_back(max.x, min.y, min.z);
	res.emplace_back(max.x, max.y, min.z);
	res.emplace_back(min.x, max.y, min.z);

	// back face
	res.emplace_back(min.x, min.y, max.z);
	res.emplace_back(max.x, min.y, max.z);
	res.emplace_back(max.x, max.y, max.z);
	res.emplace_back(min.x, max.y, max.z);

	return res;
}
}
