#pragma once

namespace toadll
{
	/// Draws a filled 3d box
	void draw3d_bbox_fill(const BBox& bbox, const Vec4& col);

	/// Draws a wireframe 3d box
	void draw3d_bbox_lines(const BBox& bbox, const Vec4& col);

	/// Draws a static 2d box
	void draw2d_bbox(const BBox& bbox, const Vec4& col_fill, const Vec4& col_line);
}