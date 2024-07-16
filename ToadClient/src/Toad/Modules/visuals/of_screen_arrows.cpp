#include "pch.h"
#include "Toad/toadll.h"

#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>

#include "of_screen_arrows.h"

using namespace toadll::math;

void toadll::COfScreenArrows::PreUpdate()
{
	CModule::PreUpdate();
}

void toadll::COfScreenArrows::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	m_directions.clear();
	for (const auto& e : MC->getPlayerList())
	{
		if (env->IsSameObject(MC->getLocalPlayer()->obj, e->obj))
			continue;

		auto entityPos = e->getPosition();
		if (lPlayer->Pos.dist(entityPos) > 20)
			continue;

		float yaw_diff = wrap_to_180(-(lPlayer->Yaw - get_angles(lPlayer->Pos, entityPos).first));
		float pitch_diff = wrap_to_180(-(lPlayer->Pitch - get_angles(lPlayer->Pos, entityPos).second));
		if (abs(yaw_diff) > 70 || abs(pitch_diff) > 80)
		{
			auto yaw_rad = glm::radians(yaw_diff);
			auto pitch_rad = glm::radians(pitch_diff);

			glm::vec2 direction = glm::normalize(glm::vec2(
				cos(pitch_rad),
				sin(yaw_rad) * cos(pitch_rad)
			));

			float rotation_angle = atan2(direction.x, direction.y);
			constexpr float circleRadius = 100.0f;
			glm::vec2 circleCenter = { g_screen_width / 2 , g_screen_height / 2};
			glm::vec2 trianglePosition = circleCenter + (rotation_angle * circleRadius);
			std::array<glm::vec2, 3> triangleVertices = {
				trianglePosition,                    // Vertex at direction on circle
				trianglePosition + glm::vec2(0, 20), // Vertex for top of triangle
				trianglePosition + glm::vec2(20, 0), // Vertex for side of triangle
			};

			rotate_triangle(triangleVertices, rotation_angle);

			triangleVertices[0] += trianglePosition;
			triangleVertices[1] += trianglePosition;
			triangleVertices[2] += trianglePosition;

			m_directions.emplace_back(
				std::array
				{
					ImVec2(triangleVertices[0].x, triangleVertices[0].y),
					ImVec2(triangleVertices[1].x, triangleVertices[1].y),
					ImVec2(triangleVertices[2].x, triangleVertices[2].y)
				}
			);
		}

	}
	SLEEP(10);
}

void toadll::COfScreenArrows::OnImGuiRender(ImDrawList* draw)
{
	for (const auto & arrow : m_directions)
	{
		//std::cout << arrow[0].x << " " << arrow[0].y << std::endl;
		//std::cout << arrow[1].x << " " << arrow[1].y << std::endl;
		//std::cout << arrow[2].x << " " << arrow[2].y << std::endl;
		draw->AddConvexPolyFilled(arrow.data(), 3, IM_COL32_WHITE);
	}
}