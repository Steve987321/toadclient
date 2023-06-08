#include "pch.h"
#include "Toad/Toad.h"
#include "esp.h"

namespace toadll {

void CEsp::Update(const std::shared_ptr<c_Entity>& lPlayer)
{
	auto ari = Minecraft->get_active_render_info();
	modelview = ari->get_modelview();
	projection = ari->get_projection();

	auto renderPartialTicks = Minecraft->get_renderPartialTick();

	//std::vector<EntityVisual> entity_list = {};
	std::vector<bbox> bboxes = {};
	
	for (const auto& entity : Minecraft->get_playerList())
	{
		if (env->IsSameObject(lPlayer->obj, entity->obj))
			continue;

		if (entity->is_invisible())
			continue;

		auto playerlasttickpos = lPlayer->get_lasttickposition();
		auto currPos = lPlayer->get_position();
		auto lpos = playerlasttickpos + (currPos - playerlasttickpos) * renderPartialTicks;

		auto lasttickpos = entity->get_lasttickposition();
		auto pos = entity->get_position();
		bbox b_box = {{}, {}};
		b_box.min.x = pos.x - 0.3f - lpos.x + -pos.x + lasttickpos.x + (pos.x - lasttickpos.x) * renderPartialTicks;
		b_box.min.y = pos.y        - lpos.y + -pos.y + lasttickpos.y + (pos.y - lasttickpos.y) * renderPartialTicks;
		b_box.min.z = pos.z - 0.3f - lpos.z + -pos.z + lasttickpos.z + (pos.z - lasttickpos.z) * renderPartialTicks;
		b_box.max.x = pos.x + 0.3f - lpos.x + -pos.x + lasttickpos.x + (pos.x - lasttickpos.x) * renderPartialTicks;
		b_box.max.y = pos.y + 1.8f - lpos.y + -pos.y + lasttickpos.y + (pos.y - lasttickpos.y) * renderPartialTicks;
		b_box.max.z = pos.z + 0.3f - lpos.z + -pos.z + lasttickpos.z + (pos.z - lasttickpos.z) * renderPartialTicks;

		bboxes.emplace_back(b_box);

		/*visual_entity.name = entity->get_name();
		entity_list.push_back(visual_entity);*/
	}

	bboxxesdud = bboxes;
	SLOW_SLEEP(1);
	//m_entity_list = entity_list;

}

void CEsp::OnRender()
{
	auto drawOutlinedBox = [](bbox boundingBox)
	{
		glBegin(3);
		glVertex3f(boundingBox.min.x, boundingBox.min.y, boundingBox.min.z);
		glVertex3f(boundingBox.max.x, boundingBox.min.y, boundingBox.min.z);
		glVertex3f(boundingBox.max.x, boundingBox.min.y, boundingBox.max.z);
		glVertex3f(boundingBox.min.x, boundingBox.min.y, boundingBox.max.z);
		glVertex3f(boundingBox.min.x, boundingBox.min.y, boundingBox.min.z);
		glEnd();

		glBegin(3);
		glVertex3f(boundingBox.min.x, boundingBox.max.y, boundingBox.min.z);
		glVertex3f(boundingBox.max.x, boundingBox.max.y, boundingBox.min.z);
		glVertex3f(boundingBox.max.x, boundingBox.max.y, boundingBox.max.z);
		glVertex3f(boundingBox.min.x, boundingBox.max.y, boundingBox.max.z);
		glVertex3f(boundingBox.min.x, boundingBox.max.y, boundingBox.min.z);
		glEnd();

		glBegin(1);
		glVertex3f(boundingBox.min.x, boundingBox.min.y, boundingBox.min.z);
		glVertex3f(boundingBox.min.x, boundingBox.max.y, boundingBox.min.z);
		glVertex3f(boundingBox.max.x, boundingBox.min.y, boundingBox.min.z);
		glVertex3f(boundingBox.max.x, boundingBox.max.y, boundingBox.min.z);
		glVertex3f(boundingBox.max.x, boundingBox.min.y, boundingBox.max.z);
		glVertex3f(boundingBox.max.x, boundingBox.max.y, boundingBox.max.z);
		glVertex3f(boundingBox.min.x, boundingBox.min.y, boundingBox.max.z);
		glVertex3f(boundingBox.min.x, boundingBox.max.y, boundingBox.max.z);
		glEnd();
	};

	auto cross = [](const vec3& a, const vec3& b) -> vec3
	{
		vec3 result;
		result.x = a.y * b.z - a.z * b.y;
		result.y = a.z * b.x - a.x * b.z;
		result.z = a.x * b.y - a.y * b.x;
		return result;
	};

	auto drawOutlinedBox2D = [&](bbox boundingBox)
	{
		// Extract the camera orientation from the modelview matrix
		vec3 cameraForward = { modelview[2], modelview[6], modelview[10] };
		vec3 cameraUp = { modelview[1], modelview[5], modelview[9] };
		vec3 cameraRight = cross(cameraForward, cameraUp);

		// Calculate the size and position of the billboarded box
		vec3 center = (boundingBox.min + boundingBox.max) * 0.5f;
		vec3 extents = (boundingBox.max - boundingBox.min) * 0.5f;

		// Calculate the vertices of the billboarded box
		vec3 vertices[4] = {
				center - cameraRight * extents.x - cameraUp * extents.y,
				center + cameraRight * extents.x - cameraUp * extents.y,
				center + cameraRight * extents.x + cameraUp * extents.y,
				center - cameraRight * extents.x + cameraUp * extents.y
		};

		// Render the billboarded box as a quad
		glBegin(GL_LINE_LOOP);
		for (const auto& vertice : vertices)
		{
			glVertex3f(vertice.x, vertice.y, vertice.z);
		}
		glEnd();
	};

	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(projection.data());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(modelview.data());

	glPushMatrix();
	glEnable(GL_LINE_SMOOTH);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDepthMask(false);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glLineWidth(1.f);

	for (const auto& bb : bboxxesdud)
	{
		drawOutlinedBox2D(bb);
	}

	glDisable(GL_BLEND);
	glDepthMask(true);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LINE_SMOOTH);
	glPopMatrix();

	glPopMatrix();
}

}