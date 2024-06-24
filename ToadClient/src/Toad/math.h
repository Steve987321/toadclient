#pragma once

#include <glm/vec2.hpp>

#include "Toad/types.h"

namespace toadll::math
{
	std::pair<float, float> get_angles(const Vec3& pos1, const Vec3& pos2);

	float wrap_to_180(float value);

	Vec3 get_cam_pos(const std::array<float, 16>& modelView);

	void rotate_triangle(std::array<Vec2, 3>& points, float rotation_rad);
	void rotate_triangle(std::array<glm::vec2, 3>& points, float rotation_rad);
}