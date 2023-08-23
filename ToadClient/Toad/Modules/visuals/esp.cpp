#include "pch.h"
#include "Toad/Toad.h"
#include "esp.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "draw_helpers.h"

using namespace toad;

namespace toadll {

void CEsp::PreUpdate()
{
	SLEEP(10);
}

void CEsp::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	if (!esp::enabled || !CVarsUpdater::IsVerified)
	{
		SLEEP(250);
		return;
	}

	auto modviewinverse = glm::inverse(glm::make_mat4(CVarsUpdater::ModelView.data()));

	auto camPos = glm::vec3(modviewinverse[3]);
	//std::cout << camPos.x << ' ' << camPos.y << ' ' << camPos.z << std::endl;
	//renderPos = MC->getActiveRenderInfo()->get_render_pos();
	//std::cout << "old: " << renderPos << std::endl;
	renderPos = {camPos.x, camPos.y, camPos.z};
	//std::cout << "new: " << renderPos << std::endl;
	playerPos = lPlayer->Pos;

	// Update our bounding boxes list
	m_bboxes = GetBBoxes();

	precise_sleep(0.01);
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
	glLineWidth(1.0f);

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

	if (esp::esp_mode == ESP_MODE::BOX2D_DYNAMIC)
	{
		for (const auto& ve : m_bboxes)
		{
			// get vertices from bounding box 
			auto vertices = GetBBoxVertices(ve.bb.min - lPos, ve.bb.max - lPos);

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

			// skip invalid worldtoscreen's 
			if ((int)minX * 10 == -10 && maxX > g_screen_width) continue;
			if ((int)minY * 10 == -10 && maxY > g_screen_height) continue;

			// convert color config
			auto line_col = ImGui::ColorConvertFloat4ToU32({ esp::line_col[0], esp::line_col[1], esp::line_col[2], esp::line_col[3] });
			auto fill_col = ImGui::ColorConvertFloat4ToU32({ esp::fill_col[0], esp::fill_col[1], esp::fill_col[2], esp::fill_col[3] });

			// draw box 

			if (esp::show_border)
			{
				// double sided border (inside and outside outline)
				draw->AddRect({ minX - 1, minY - 1 }, { maxX + 1, maxY + 1 }, IM_COL32_BLACK);
				draw->AddRect({ minX + 1, minY + 1 }, { maxX - 1, maxY - 1 }, IM_COL32_BLACK);
			}

			draw->AddRectFilled({ minX - 1, minY - 1 }, { maxX + 1, maxY + 1 }, fill_col);
			draw->AddRect({ minX, minY }, { maxX, maxY }, line_col);

			drawPlayerInfo(draw, ve, lPos);
		}
	}
	else
	{
		for (const auto& ve : m_bboxes)
		{
			drawPlayerInfo(draw, ve, lPos);
		}
	}
	
}


void CEsp::drawPlayerInfo(ImDrawList* draw, const VisualEntity& ve, const Vec3& lPlayerPos)
{
	if (esp::show_name || esp::show_distance || esp::show_health)
	{
		// the center and top of player 
		auto infoPos = (ve.bb.min + ve.bb.max) * 0.5f;
		infoPos.y += 1.f;

		auto screenpos = world_to_screen(infoPos - lPlayerPos, renderPos);

		if ((int)screenpos.x * 10 != -10 && (int)screenpos.y * 10 != -10)
		{
			const auto font = HSwapBuffers::GetFont();
			char text[30] = {};

			if (esp::show_name)
			{
				strncat_s(text, ve.name.c_str(), ve.name.length());
			}
			if (esp::show_distance)
			{
				auto distStr = " [" + std::to_string(playerPos.dist(ve.Pos)).substr(0, 3) + ']';
				strncat_s(text, distStr.c_str(), distStr.length());
			}

			// draw our text

			const auto textsize = font->CalcTextSizeA(esp::text_size, 500, 0, text);

			if (esp::show_txt_bg)
			{
				draw->AddRectFilled(
					{ screenpos.x - textsize.x / 2, screenpos.y - 5 - textsize.y},
					{ screenpos.x + textsize.x / 2, screenpos.y - 5},
					ImGui::GetColorU32({ esp::text_bg_col[0], esp::text_bg_col[1], esp::text_bg_col[2], esp::text_bg_col[3] })
				);
			}

			auto text_col_imu32 = ImGui::ColorConvertFloat4ToU32({ esp::text_col[0], esp::text_col[1], esp::text_col[2], esp::text_col[3] });

			// text position (positioned inside the background box)
			auto boxPosYText = std::lerp(screenpos.y - 5 - textsize.y, screenpos.y - 5, 0.9f) - esp::text_size;

			if (esp::text_shadow)
			{
				draw->AddText(font, esp::text_size, { screenpos.x - textsize.x / 2 - 1, boxPosYText - 1 }, IM_COL32_BLACK, text);
				draw->AddText(font, esp::text_size, { screenpos.x - textsize.x / 2 + 1, boxPosYText + 1 }, IM_COL32_BLACK, text);
			}

			draw->AddText(font, esp::text_size, { screenpos.x - textsize.x / 2, boxPosYText }, text_col_imu32, text);

			if (esp::show_health)
			{
				// get vertices from bounding box 
				auto vertices = GetBBoxVertices(ve.bb.min - lPlayerPos, ve.bb.max - lPlayerPos);

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

				if ((int)minX * 10 == -10 && maxX > g_screen_width) return;
				if ((int)minY * 10 == -10 && maxY > g_screen_height) return;

				float t = ((float)ve.health / 20.f);

				if (t < 0) t = 0;

				// t starts with 1 
				ImVec2 rightTop = { maxX + 3.f, std::lerp(maxY, minY, t) };
				ImVec2 rightDown = { maxX + 5.f, maxY };
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

				if (esp::show_border)
				{
					draw->AddRectFilled({rightTop.x - 1, rightTop.y - 1}, {rightDown.x + 1, rightDown.y + 1}, IM_COL32_BLACK);
				}

				// top: green -> middle: yellow -> bottom: red
				draw->AddRectFilled(rightTop, rightDown, ImGui::GetColorU32(col));
			}
		}
	}
}

std::vector<CEsp::VisualEntity> CEsp::GetBBoxes()
{
	std::vector<VisualEntity> res = {};

	auto lPlayer = MC->getLocalPlayer();

	for (const auto& entity : MC->getPlayerList())
	{
		if (env->IsSameObject(lPlayer->obj, entity->obj))
			continue;
		if (entity->isInvisible())
			continue;

		auto pos = entity->getPosition();

		auto campos = lPlayer->getPosition() + renderPos;
		campos.y -= 1.0f;
		//std::cout << campos << " - " << pos << " = " << (pos.dist(campos)) << std::endl;
		if (pos.dist(campos) < 1.f)
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


		res.emplace_back(b_box, pos, esp::show_name ? entity->getName() : "", esp::show_health ? entity->getHealth() : -1, 0);
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
