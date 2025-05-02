#pragma once

#include <glm/vec2.hpp>

#include "Toad/types.h"
#include <unordered_set>

namespace toad::math
{
	struct Rotation
	{
		float yaw; 
		float pitch;
	};

	Rotation get_angles(const Vec3& pos1, const Vec3& pos2);

	float wrap_to_180(float value);

	Vec3 get_cam_pos(const std::array<double, 16>& modelView);

	void rotate_triangle(std::array<Vec2, 3>& points, float rotation_rad);
	void rotate_triangle(std::array<glm::vec2, 3>& points, float rotation_rad);

	template<typename T>
	float jaccard_index(const std::vector<T>& a, const std::vector<T>& b)
	{
		std::unordered_set<T> set_a(a.begin(), a.end());
		std::unordered_set<T> set_b(b.begin(), b.end());

		size_t intersection_size = 0;
		size_t union_size = set_a.size();

		for (const T& elem : set_b)
		{
			if (set_a.count(elem))
				intersection_size++;
			else
				union_size++;
		}

		if (union_size == 0)
			return 1.f;

		return (float)intersection_size / union_size;
	}
}