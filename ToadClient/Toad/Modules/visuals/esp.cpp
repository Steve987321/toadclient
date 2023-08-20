#include "pch.h"
#include "Toad/Toad.h"
#include "esp.h"

#include "draw_helpers.h"

using namespace toad;

namespace toadll {

void CEsp::PreUpdate()
{
	SLEEP(10);
	//precise_sleep(0.005);
}

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

	if (esp::esp_mode == ESP_MODE::BOX2D_DYNAMIC)
		return;

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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glLineWidth(esp::line_width);

	auto lPos = CVarsUpdater::theLocalPlayer->LastTickPos + (CVarsUpdater::theLocalPlayer->Pos - CVarsUpdater::theLocalPlayer->LastTickPos) * CVarsUpdater::RenderPartialTick;

	//m_bboxesMutex.lock();
	for (const auto& e : m_bboxes)
	{
		auto bb = BBox{e.bb.min - lPos, e.bb.max - lPos};

		switch (esp::esp_mode)
		{
		case ESP_MODE::BOX3D:
			draw3d_bbox_fill(
				bb,
				{ esp::fill_col[0], esp::fill_col[1], esp::fill_col[2], esp::fill_col[3] }
			);
			draw3d_bbox_lines(
				bb,
				{ esp::line_col[0], esp::line_col[1], esp::line_col[2], esp::line_col[3] }
			);
			break;

		case ESP_MODE::BOX2D_DYNAMIC: // uses imgui 
			break;

		case ESP_MODE::BOX2D_STATIC:
			draw2d_bbox(
				bb,
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

	auto lPos = CVarsUpdater::theLocalPlayer->LastTickPos + (CVarsUpdater::theLocalPlayer->Pos - CVarsUpdater::theLocalPlayer->LastTickPos) * CVarsUpdater::RenderPartialTick;
	const auto font = HSwapBuffers::GetFont();

	if (esp::esp_mode == ESP_MODE::BOX2D_DYNAMIC)
	{
		for (const auto& [bb, pos, name, health, hurttime] : m_bboxes)
		{
			// get vertices from bounding box 
			auto vertices = GetBBoxVertices(bb.min - lPos, bb.max - lPos);

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

			// draw box 
			draw->AddRectFilled({ minX, minY }, { maxX, maxY }, fill_col);
			draw->AddRect({ minX - esp::line_width, minY - esp::line_width }, { maxX + esp::line_width, maxY + esp::line_width }, line_col);

			if (esp::show_name || esp::show_distance || esp::show_health)
			{
				// get the center top of player 
				auto posAddedY = (bb.min + bb.max) * 0.5f;
				posAddedY.y += 2.f;

				auto screenpos = world_to_screen(posAddedY - lPos, renderPos);
				auto text_col = ImGui::ColorConvertFloat4ToU32({ esp::text_col[0], esp::text_col[1], esp::text_col[2], esp::text_col[3] });

				if ((int)screenpos.x * 10 != -10 && (int)screenpos.y * 10 != -10)
				{
					if (esp::show_name)
					{
						auto textSize = font->CalcTextSizeA(esp::text_size, 5000.f, 0, name.c_str());
						auto posX = screenpos.x - textSize.x / 2;

						if (esp::text_shadow)
						{
							draw->AddText(font, esp::text_size, { posX - 1, screenpos.y - 1 }, IM_COL32_BLACK, name.c_str());
							draw->AddText(font, esp::text_size, { posX + 1, screenpos.y + 1 }, IM_COL32_BLACK, name.c_str());
						}
						
						draw->AddText(font, esp::text_size, { posX, screenpos.y }, text_col, name.c_str());
					}
					if (esp::show_distance)
					{
						auto distStr = std::to_string(playerPos.dist(pos)).substr(0, 3);
						auto textSize = ImGui::CalcTextSize(distStr.c_str());
						auto posX = screenpos.x - textSize.x / 2;

						if (esp::text_shadow)
						{
							draw->AddText({ posX - 1, screenpos.y + 10 - 1 }, IM_COL32_BLACK, distStr.c_str());
							draw->AddText({ posX + 1, screenpos.y + 10 + 1 }, IM_COL32_BLACK, distStr.c_str());
						}
						draw->AddText({ posX, screenpos.y + 10 }, text_col, distStr.c_str());
					}
					if (esp::show_health)
					{
						if ((int)minX * 10 == -10 && maxX > g_screen_width) continue;
						if ((int)minY * 10 == -10 && maxY > g_screen_height) continue;

						float t = ((float)health / 20.f);

						// t starts with 1 
						ImVec2 rightTop = { maxX + 2.f, std::lerp(maxY, minY, t) };
						ImVec2 rightDown = { maxX + 4.f, maxY };
						ImVec4 col = { 0,0,0,1 };

						if (t > 0.5f)
						{
							// to yellow
							col.y = std::lerp(0.f, 1.f, t / 0.5f - 1);
							col.x = std::lerp(1.f, 0.f, t / 0.5f - 1);
						}
						else
						{
							// to red
							col.x = std::lerp(1.f, 0.f, t / 0.5f - 1);
							col.y = std::lerp(0.f, 1.f, t / 0.5f - 1);
						}

						// top: green -> middle: yellow -> bottom: red
						draw->AddRectFilled(rightTop, rightDown, ImGui::GetColorU32(col));
					}
				}
			}
		}
	}
	else
	{
		if (esp::show_name || esp::show_distance || esp::show_health)
		{
			for (const auto& [bb, pos, name, health, hurttime] : m_bboxes)
			{
				// get the center and top of player 
				auto posAddedY = (bb.min + bb.max) * 0.5f;
				posAddedY.y += 2.f;

				auto screenpos = world_to_screen(posAddedY - lPos, renderPos);
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
					if (esp::show_health)
					{
						// pasted from the dynamic 2d bbox

						// get vertices from bounding box 
						auto vertices = GetBBoxVertices(bb.min - lPos, bb.max - lPos);

						std::vector<Vec2> verticesScreenPos = {};

						// world vertices pos to screen pos
						for (const auto& v : vertices)
						{
							auto screen = world_to_screen(v, renderPos);
							verticesScreenPos.emplace_back(screen);
						}

						auto minX = std::ranges::min_element(verticesScreenPos, [](const Vec2& a, const Vec2& b) { return a.x < b.x; })->x;
						auto minY = std::ranges::min_element(verticesScreenPos, [](const Vec2& a, const Vec2& b) { return a.y < b.y; })->y;
						auto maxX = std::ranges::max_element(verticesScreenPos, [](const Vec2& a, const Vec2& b) { return a.x < b.x; })->x;
						auto maxY = std::ranges::max_element(verticesScreenPos, [](const Vec2& a, const Vec2& b) { return a.y < b.y; })->y;

						if ((int)minX * 10 == -10 && maxX > g_screen_width) continue;
						if ((int)minY * 10 == -10 && maxY > g_screen_height) continue;

						float t = ((float)health / 20.f);

						// t starts with 1 
						ImVec2 rightTop = { maxX + 2.f, std::lerp(maxY, minY, t) };
						ImVec2 rightDown = { maxX + 4.f, maxY };
						ImVec4 col = { 0,0,0,1 };

						if (t > 0.5f)
						{
							// to yellow
							col.x = std::lerp(1.f, 0.f, t / 0.5f - 1);
							col.y = 1;
						}
						else
						{
							// to red
							col.x = 1;
							col.y = std::lerp(0.f, 1.f, t / 0.5f);
						}

						// top: green -> middle: yellow -> bottom: red
						draw->AddRectFilled(rightTop, rightDown, ImGui::GetColorU32(col));
					}
				}
			}
		}
	}
}

std::vector<CEsp::VisualEntity> CEsp::GetBBoxes()
{
	std::vector<VisualEntity> res = {};

	for (const auto& entity : MC->getPlayerList())
	{
		if (entity->isInvisible())
			continue;

		auto pos = entity->getPosition();

		if (pos.dist(MC->getLocalPlayer()->getPosition()) < 0.5f)
			continue;

		// local player 
		//auto lPlayer = MC->getLocalPlayer();
		//auto playerlasttickpos = lPlayer->getLastTickPosition();
		//auto currPos = lPlayer->getPosition();

		//auto lpos = playerlasttickpos + (currPos - playerlasttickpos) * CVarsUpdater::RenderPartialTick;
		auto lasttickpos = entity->getLastTickPosition();
		BBox b_box = { {}, {} };
		b_box.min.x = lasttickpos.x + (pos.x - lasttickpos.x) * CVarsUpdater::RenderPartialTick - 0.3f;
		b_box.min.y = lasttickpos.y + (pos.y - lasttickpos.y) * CVarsUpdater::RenderPartialTick;
		b_box.min.z = lasttickpos.z + (pos.z - lasttickpos.z) * CVarsUpdater::RenderPartialTick - 0.3f;
		b_box.max.x = lasttickpos.x + (pos.x - lasttickpos.x) * CVarsUpdater::RenderPartialTick + 0.3f;
		b_box.max.y = lasttickpos.y + (pos.y - lasttickpos.y) * CVarsUpdater::RenderPartialTick + 1.8f;
		b_box.max.z = lasttickpos.z + (pos.z - lasttickpos.z) * CVarsUpdater::RenderPartialTick + 0.3f;

		res.emplace_back(b_box, pos, entity->getName(), entity->getHealth(), entity->getHurtTime());
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
