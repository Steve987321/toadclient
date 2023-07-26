#include "pch.h"
#include "Toad/Toad.h"
#include "draw_helpers.h"

namespace toadll
{
	void draw3d_bbox_lines(const BBox& bbox, const Vec4& col)
	{
		glBegin(3);
		glColor4f(col.x, col.y, col.z, col.w);
		glVertex3f(bbox.min.x, bbox.min.y, bbox.min.z);
		glVertex3f(bbox.max.x, bbox.min.y, bbox.min.z);
		glVertex3f(bbox.max.x, bbox.min.y, bbox.max.z);
		glVertex3f(bbox.min.x, bbox.min.y, bbox.max.z);
		glVertex3f(bbox.min.x, bbox.min.y, bbox.min.z);
		glEnd();

		glBegin(3);
		glColor4f(col.x, col.y, col.z, col.w);
		glVertex3f(bbox.min.x, bbox.max.y, bbox.min.z);
		glVertex3f(bbox.max.x, bbox.max.y, bbox.min.z);
		glVertex3f(bbox.max.x, bbox.max.y, bbox.max.z);
		glVertex3f(bbox.min.x, bbox.max.y, bbox.max.z);
		glVertex3f(bbox.min.x, bbox.max.y, bbox.min.z);
		glEnd();

		glBegin(1);
		glColor4f(col.x, col.y, col.z, col.w);
		glVertex3f(bbox.min.x, bbox.min.y, bbox.min.z);
		glVertex3f(bbox.min.x, bbox.max.y, bbox.min.z);
		glVertex3f(bbox.max.x, bbox.min.y, bbox.min.z);
		glVertex3f(bbox.max.x, bbox.max.y, bbox.min.z);
		glVertex3f(bbox.max.x, bbox.min.y, bbox.max.z);
		glVertex3f(bbox.max.x, bbox.max.y, bbox.max.z);
		glVertex3f(bbox.min.x, bbox.min.y, bbox.max.z);
		glVertex3f(bbox.min.x, bbox.max.y, bbox.max.z);
		glEnd();
	}

	void draw3d_bbox_fill(const BBox& bbox, const Vec4& col)
	{
		glBegin(GL_QUADS);

		glColor4f(col.x, col.y, col.z, col.w);
		glVertex3f(bbox.min.x, bbox.min.y, bbox.min.z);
		glVertex3f(bbox.min.x, bbox.max.y, bbox.min.z);
		glVertex3f(bbox.max.x, bbox.max.y, bbox.min.z);
		glVertex3f(bbox.max.x, bbox.min.y, bbox.min.z);

		glVertex3f(bbox.max.x, bbox.min.y, bbox.max.z);
		glVertex3f(bbox.max.x, bbox.max.y, bbox.max.z);
		glVertex3f(bbox.min.x, bbox.max.y, bbox.max.z);
		glVertex3f(bbox.min.x, bbox.min.y, bbox.max.z);

		glVertex3f(bbox.min.x, bbox.min.y, bbox.min.z);
		glVertex3f(bbox.min.x, bbox.min.y, bbox.max.z);
		glVertex3f(bbox.min.x, bbox.max.y, bbox.max.z);
		glVertex3f(bbox.min.x, bbox.max.y, bbox.min.z);

		glVertex3f(bbox.max.x, bbox.min.y, bbox.max.z);
		glVertex3f(bbox.max.x, bbox.min.y, bbox.min.z);
		glVertex3f(bbox.max.x, bbox.max.y, bbox.min.z);
		glVertex3f(bbox.max.x, bbox.max.y, bbox.max.z);

		glVertex3f(bbox.min.x, bbox.min.y, bbox.min.z);
		glVertex3f(bbox.max.x, bbox.min.y, bbox.min.z);
		glVertex3f(bbox.max.x, bbox.min.y, bbox.max.z);
		glVertex3f(bbox.min.x, bbox.min.y, bbox.max.z);

		glVertex3f(bbox.min.x, bbox.max.y, bbox.max.z);
		glVertex3f(bbox.max.x, bbox.max.y, bbox.max.z);
		glVertex3f(bbox.max.x, bbox.max.y, bbox.min.z);
		glVertex3f(bbox.min.x, bbox.max.y, bbox.min.z);

		glEnd();
	}

	void draw2d_bbox(const BBox& bbox, const Vec4& col_fill, const Vec4& col_line)
	{
		Vec3 cameraForward = { CVarsUpdater::ModelView[2], CVarsUpdater::ModelView[6], CVarsUpdater::ModelView[10] };
		Vec3 cameraUp = { CVarsUpdater::ModelView[1], CVarsUpdater::ModelView[5], CVarsUpdater::ModelView[9] };
		Vec3 cameraRight = cameraForward.cross(cameraUp);

		Vec3 center = (bbox.min + bbox.max) * 0.5f;
		Vec3 extents = (bbox.max - bbox.min) * 0.5f;

		Vec3 vertices[4] = {
				center + cameraRight * extents.x - cameraUp * extents.y,
				center - cameraRight * extents.x - cameraUp * extents.y,
				center - cameraRight * extents.x + cameraUp * extents.y,
				center + cameraRight * extents.x + cameraUp * extents.y
		};

		// fill
		glColor4f(col_fill.x, col_fill.y, col_fill.z, col_fill.w);
		glBegin(GL_QUADS);
		for (const auto& vertice : vertices)
		{
			glVertex3f(vertice.x, vertice.y, vertice.z);
		}
		glEnd();

		// outlines
		glColor4f(col_line.x, col_line.y, col_line.z, col_line.w);
		glBegin(GL_LINE_LOOP);
		for (const auto& vertice : vertices)
		{
			glVertex3f(vertice.x, vertice.y, vertice.z);
		}
		glEnd();
	}

}
