#pragma once

#include "Toad/Types.h"

namespace toadll::math
{
	std::pair<float, float> get_angles(const Vec3& pos1, const Vec3& pos2);

	float wrap_to_180(float value);

	Vec3 get_cam_pos(const std::array<float, 16>& modelView);
}