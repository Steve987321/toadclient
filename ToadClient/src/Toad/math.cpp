#include "pch.h"
#include "math.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace toadll::math
{

	std::pair<float, float> get_angles(const Vec3& pos1, const Vec3& pos2)
	{
		float d_x = pos2.x - pos1.x;
		float d_y = pos2.y - pos1.y;
		float d_z = pos2.z - pos1.z;

		float hypothenuse = sqrt(d_x * d_x + d_z * d_z);
		float yaw = atan2(d_z, d_x) * 180.f / g_PI - 90.f;
		float pitch = -atan2(d_y, hypothenuse) * 180 / g_PI;

		return std::make_pair(yaw, pitch);
	}

	float wrap_to_180(float value)
	{
		float res = std::fmodf(value + 180, 360);
		if (res < 0)
			res += 360;
		return res - 180;
	}

	Vec3 get_cam_pos(const std::array<float, 16>& modelView)
	{
		auto modviewinverse = glm::inverse(glm::make_mat4(modelView.data()));
		auto pos = glm::vec3(modviewinverse[3]);
		return {pos.x, pos.y, pos.z};
	}

	void rotate_triangle(std::array<Vec2, 3>& points, float rotation_rad)
	{
		auto points_center = (points.at(0) + points.at(1) + points.at(2)) / 3;
		for (auto& point : points)
		{
			const auto temp_x = point.x - points_center.x;
			const auto temp_y = point.y - points_center.y;

			const auto c = cosf(rotation_rad);
			const auto s = sinf(rotation_rad);

			point.x = temp_x * c - temp_y * s;
			point.y = temp_x * s + temp_y * c;
		}
	}

	void rotate_triangle(std::array<glm::vec2, 3>& points, float rotation_rad)
	{
		auto points_center = (points.at(0) + points.at(1) + points.at(2));
		points_center /= 3;
		for (auto& point : points)
		{
			const auto temp_x = point.x - points_center.x;
			const auto temp_y = point.y - points_center.y;

			const auto c = cosf(rotation_rad);
			const auto s = sinf(rotation_rad);

			point.x = temp_x * c - temp_y * s;
			point.y = temp_x * s + temp_y * c;
		}
	}

}

