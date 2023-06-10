#pragma once
#include <gl/GLU.h>

namespace toadll
{

class CEsp SET_MODULE_CLASS(CEsp)
{
private:
	struct EntityVisual
	{
		std::string name = "?";
		bbox b_box = {{0,0,0}, {0,0,0}};
		vec3 lasttickpos = {0,0,0};
		vec3 position = { 0,0,0 };
	};

private:
	inline static std::vector<bbox> bboxxesdud;
	inline static std::vector<float> modelview;
	inline static std::vector<float> projection;

	std::vector<EntityVisual> m_entity_list;

private:
	static inline void draw3dBox(const bbox& b_box)
	{
		glBegin(3);
		glColor4f(toad::esp::lineCol[0], toad::esp::lineCol[1], toad::esp::lineCol[2], toad::esp::lineCol[3]);
		glVertex3f(b_box.min.x, b_box.min.y, b_box.min.z);
		glVertex3f(b_box.max.x, b_box.min.y, b_box.min.z);
		glVertex3f(b_box.max.x, b_box.min.y, b_box.max.z);
		glVertex3f(b_box.min.x, b_box.min.y, b_box.max.z);
		glVertex3f(b_box.min.x, b_box.min.y, b_box.min.z);
		glEnd();

		glBegin(3);
		glColor4f(toad::esp::lineCol[0], toad::esp::lineCol[1], toad::esp::lineCol[2], toad::esp::lineCol[3]);
		glVertex3f(b_box.min.x, b_box.max.y, b_box.min.z);
		glVertex3f(b_box.max.x, b_box.max.y, b_box.min.z);
		glVertex3f(b_box.max.x, b_box.max.y, b_box.max.z);
		glVertex3f(b_box.min.x, b_box.max.y, b_box.max.z);
		glVertex3f(b_box.min.x, b_box.max.y, b_box.min.z);
		glEnd();

		glBegin(1);
		glColor4f(toad::esp::lineCol[0], toad::esp::lineCol[1], toad::esp::lineCol[2], toad::esp::lineCol[3]);
		glVertex3f(b_box.min.x, b_box.min.y, b_box.min.z);
		glVertex3f(b_box.min.x, b_box.max.y, b_box.min.z);
		glVertex3f(b_box.max.x, b_box.min.y, b_box.min.z);
		glVertex3f(b_box.max.x, b_box.max.y, b_box.min.z);
		glVertex3f(b_box.max.x, b_box.min.y, b_box.max.z);
		glVertex3f(b_box.max.x, b_box.max.y, b_box.max.z);
		glVertex3f(b_box.min.x, b_box.min.y, b_box.max.z);
		glVertex3f(b_box.min.x, b_box.max.y, b_box.max.z);
		glEnd();
	}

	static inline void draw2dBox(const bbox& b_box)
	{
		vec3 cameraForward = { modelview[2], modelview[6], modelview[10] };
		vec3 cameraUp = { modelview[1], modelview[5], modelview[9] };
		vec3 cameraRight = cameraForward.cross(cameraUp);

		vec3 center = (b_box.min + b_box.max) * 0.5f;
		vec3 extents = (b_box.max - b_box.min) * 0.5f;

		vec3 vertices[4] = {
				center - cameraRight * extents.x - cameraUp * extents.y,
				center + cameraRight * extents.x - cameraUp * extents.y,
				center + cameraRight * extents.x + cameraUp * extents.y,
				center - cameraRight * extents.x + cameraUp * extents.y
		};
		vec3 vertices2[4] = {
				vec3(center.x - extents.x, center.y - extents.y, center.z),
				vec3(center.x + extents.x, center.y - extents.y, center.z),
				vec3(center.x + extents.x, center.y + extents.y, center.z),
				vec3(center.x - extents.x, center.y + extents.y, center.z)
		};

		//glBegin(GL_QUADS);
		//glColor4f(toad::esp::fillCol[0], toad::esp::fillCol[1], toad::esp::fillCol[2], toad::esp::fillCol[3]);
		//glVertex2f(left, top);
		//glVertex2f(right, top);
		//glVertex2f(right, bottom);
		//glVertex2f(left, bottom);
		//glEnd();

		// outlines
		glColor4f(toad::esp::lineCol[0], toad::esp::lineCol[1], toad::esp::lineCol[2], toad::esp::lineCol[3]);
		glBegin(GL_LINE_LOOP);
		for (const auto& vertice : vertices)
		{
			glVertex3f(vertice.x, vertice.y, vertice.z);
		}
		glEnd();

	}

public:
	void Update(const std::shared_ptr<c_Entity>& lPlayer) override;
	void OnRender() override;
};

}