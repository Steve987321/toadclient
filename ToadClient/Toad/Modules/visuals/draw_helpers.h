#pragma once

namespace toadll
{
	void draw3d_box_fill(const BBox& bbox, const Vec4& col);

	void draw3d_box_lines(const BBox& bbox, const Vec4& col);

	void draw2dBox(const BBox& bbox, const Vec4& col_fill, const Vec4& col_line);
}