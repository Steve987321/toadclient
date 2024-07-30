#pragma once

#include <glm/vec2.hpp>

#include "Toad/types.h"

namespace toadll::math
{
	std::pair<float, float> get_angles(const Vec3& pos1, const Vec3& pos2);

	float wrap_to_180(float value);

	Vec3 get_cam_pos(const std::array<double, 16>& modelView);

	void rotate_triangle(std::array<Vec2, 3>& points, float rotation_rad);
	void rotate_triangle(std::array<glm::vec2, 3>& points, float rotation_rad);

	template<typename T>
	float jaccard_index(const std::vector<T>& a, const std::vector<T>& b)
	{
		std::set<T> set_a(a.begin(), a.end());
		std::set<T> set_b(b.begin(), b.end());

		std::vector<T> intersection_vec;
		std::vector<T> union_vec;

		std::set_intersection(set_a.begin(), set_a.end(), set_b.begin(), set_b.end(), std::back_inserter(intersection_vec));
		std::set_union(set_a.begin(), set_a.end(), set_b.begin(), set_b.end(), std::back_inserter(union_vec));

		if (union_vec.empty())
			return 1.f;
		

		return (float)intersection_vec.size() / union_vec.size();
	}
}