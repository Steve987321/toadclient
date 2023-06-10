#include "pch.h"
#include "Toad/Toad.h"
#include "esp.h"

using namespace toad;

namespace toadll {

void CEsp::Update(const std::shared_ptr<c_Entity>& lPlayer)
{
	if (!esp::enabled)
	{
		SLOW_SLEEP(200);
		return;
	}

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
	if (!esp::enabled) return;

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
		draw2dBox(bb);
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