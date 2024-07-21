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

	template<typename T>
	float jaccard_index(const std::vector<T>& a, const std::vector<T>& b)
	{
		std::vector<uint8_t> intersection_vec;
		std::vector<uint8_t> union_vec;

		std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(intersection_vec));
		std::set_union(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(union_vec));

		if (union_vec.empty()) 
			return 1.f; 
		
		return (float)(intersection_vec.size() / union_vec.size());
	}
}