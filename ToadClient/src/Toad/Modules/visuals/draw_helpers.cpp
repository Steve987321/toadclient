#include "pch.h"
#include "Toad/Toad.h"
#include "draw_helpers.h"

namespace toadll
{
	void draw3d_bbox_lines(const BBox& bbox, const Vec4& col)
	{
		glBegin(GL_LINE_LOOP);
		glColor4f(col.x, col.y, col.z, col.w);
		glVertex3f(bbox.min.x, bbox.min.y, bbox.min.z);
		glVertex3f(bbox.max.x, bbox.min.y, bbox.min.z);
		glVertex3f(bbox.max.x, bbox.min.y, bbox.max.z);
		glVertex3f(bbox.min.x, bbox.min.y, bbox.max.z);
		glVertex3f(bbox.min.x, bbox.min.y, bbox.min.z);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glColor4f(col.x, col.y, col.z, col.w);
		glVertex3f(bbox.min.x, bbox.max.y, bbox.min.z);
		glVertex3f(bbox.max.x, bbox.max.y, bbox.min.z);
		glVertex3f(bbox.max.x, bbox.max.y, bbox.max.z);
		glVertex3f(bbox.min.x, bbox.max.y, bbox.max.z);
		glVertex3f(bbox.min.x, bbox.max.y, bbox.min.z);
		glEnd();

		glBegin(GL_LINES);
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

	Vec2 world_to_screen(const Vec3& world_pos, const Vec3& cam_pos)
	{
		GLdouble modelview[16];
		GLdouble projection[16];

		// convert to double types 
		for (int i = 0; i < 16; i++)
		{
			modelview[i] = static_cast<GLdouble>(CVarsUpdater::ModelView[i]);
			projection[i] = static_cast<GLdouble>(CVarsUpdater::Projection[i]);
		}

		double screenX, screenY, screenZ;
		gluProject(world_pos.x, world_pos.y, world_pos.z,
			modelview, projection, CVarsUpdater::Viewport.data(),
			&screenX, &screenY, &screenZ);

		Vec3 cam_forward = { CVarsUpdater::ModelView[2], CVarsUpdater::ModelView[6], CVarsUpdater::ModelView[10] };
		Vec3 cam_pos_relative = world_pos - cam_pos;
		float dot = cam_pos_relative.dot(Vec3::normalize(cam_forward));
		
		// check if worldPos is in front of the camera
		if (dot < 0) {
			return { static_cast<float>(screenX), static_cast<float>(CVarsUpdater::Viewport[3] - screenY) };
		}

		// is behind the camera 
		return { -1.0f, -1.0f };

		//return { static_cast<float>(screenX), static_cast<float>(viewport[3] - screenY) };
	}

	Vec2 world_to_screen_no_behind_test(const Vec3& world_pos, const Vec3& camera_pos)
	{
		GLdouble modelview[16];
		GLdouble projection[16];

		// convert to double types 
		for (int i = 0; i < 16; i++)
		{
			modelview[i] = static_cast<GLdouble>(CVarsUpdater::ModelView[i]);
			projection[i] = static_cast<GLdouble>(CVarsUpdater::Projection[i]);
		}

		double screenX, screenY, screenZ;
		gluProject(world_pos.x, world_pos.y, world_pos.z,
			modelview, projection, CVarsUpdater::Viewport.data(),
			&screenX, &screenY, &screenZ);

		Vec3 cam_forward = { CVarsUpdater::ModelView[2], CVarsUpdater::ModelView[6], CVarsUpdater::ModelView[10] };
		Vec3 cam_pos_relative = world_pos - camera_pos;
		float dot = cam_pos_relative.dot(Vec3::normalize(cam_forward));
		//std::cout << dotProduct << std::endl;
		// check if worldPos is in front of the camera
		if (dot > 0)
		{
			return { static_cast<float>(screenX), static_cast<float>(CVarsUpdater::Viewport[3] - screenY) };
		}
		return { -1, -1 };
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
		Vec3 cam_forward = { CVarsUpdater::ModelView[2], CVarsUpdater::ModelView[6], CVarsUpdater::ModelView[10] };
		Vec3 cam_up = { CVarsUpdater::ModelView[1], CVarsUpdater::ModelView[5], CVarsUpdater::ModelView[9] };
		Vec3 cam_right = cam_forward.cross(cam_up);

		Vec3 center = (bbox.min + bbox.max) * 0.5f;
		Vec3 extents = (bbox.max - bbox.min) * 0.5f;
		extents.x += toad::esp::static_esp_width;

		Vec3 vertices[4] = {
				center + cam_right * extents.x - cam_up * extents.y,
				center - cam_right * extents.x - cam_up * extents.y,
				center - cam_right * extents.x + cam_up * extents.y,
				center + cam_right * extents.x + cam_up * extents.y
		};

		// fill
		glColor4f(col_fill.x, col_fill.y, col_fill.z, col_fill.w);
		glBegin(GL_QUADS);
		for (const auto& v : vertices)
		{
			glVertex3f(v.x, v.y, v.z);
		}
		glEnd();

		// outlines
		glColor4f(col_line.x, col_line.y, col_line.z, col_line.w);
		glBegin(GL_LINE_LOOP);
		for (const auto& v : vertices)
		{
			glVertex3f(v.x, v.y, v.z);
		}
		glEnd();
	}

}
