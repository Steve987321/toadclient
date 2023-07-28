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

	Vec2 world_to_screen(const Vec3& worldPos)
	{
		GLint viewport[4];

		GLdouble modelview[16];
		GLdouble projection[16];

		// convert to double types 
		for (int i = 0; i < 16; i++)
		{
			modelview[i] = static_cast<GLdouble>(CVarsUpdater::ModelView[i]);
			projection[i] = static_cast<GLdouble>(CVarsUpdater::Projection[i]);
		}

		glGetIntegerv(GL_VIEWPORT, viewport);

		double screenX, screenY, screenZ;
		gluProject(worldPos.x, worldPos.y, worldPos.z,
			modelview, projection, viewport,
			&screenX, &screenY, &screenZ);

		Vec3 cameraPos = { CVarsUpdater::ModelView[12], CVarsUpdater::ModelView[13], CVarsUpdater::ModelView[14] };
		Vec3 cameraForward = { CVarsUpdater::ModelView[2], CVarsUpdater::ModelView[6], CVarsUpdater::ModelView[10] };
		Vec3 cameraToProjected = { worldPos.x - cameraPos.x, worldPos.y - cameraPos.y, worldPos.z - cameraPos.z };
		float dotProduct = cameraToProjected.dot(cameraForward);

		// check if worldPos is in front of the camera
		if (dotProduct < 0) {
			return { static_cast<float>(screenX), static_cast<float>(viewport[3] - screenY) };
		}

		// is behind the camera 
		return { -1.0f, -1.0f };

		//return { static_cast<float>(screenX), static_cast<float>(viewport[3] - screenY) };
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
