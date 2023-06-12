#include "pch.h"
#include "Toad/Toad.h"
#include "esp.h"

using namespace toad;

namespace toadll {

void CEsp::Update(const std::shared_ptr<LocalPlayerT>& lPlayer)
{
	if (!esp::enabled)
	{
		SLOW_SLEEP(200);
		return;
	}

	//std::vector<EntityVisual> entity_list = {};
	std::vector<bbox> bboxes = {};

	for (const auto& entity : CVarsUpdater::PlayerList)
	{
		if (entity->Invis)
			continue;

		auto playerlasttickpos = lPlayer->LastTickPos;
		auto currPos = lPlayer->Pos;
		auto lpos = playerlasttickpos + (currPos - playerlasttickpos) * CVarsUpdater::RenderPartialTick;

		auto lasttickpos = entity->LastTickPos;
		auto pos = entity->Pos;

		bbox b_box = {{}, {}};
		b_box.min.x = pos.x - 0.3f - lpos.x + -pos.x + lasttickpos.x + (pos.x - lasttickpos.x) * CVarsUpdater::RenderPartialTick;
		b_box.min.y = pos.y        - lpos.y + -pos.y + lasttickpos.y + (pos.y - lasttickpos.y) * CVarsUpdater::RenderPartialTick;
		b_box.min.z = pos.z - 0.3f - lpos.z + -pos.z + lasttickpos.z + (pos.z - lasttickpos.z) * CVarsUpdater::RenderPartialTick;
		b_box.max.x = pos.x + 0.3f - lpos.x + -pos.x + lasttickpos.x + (pos.x - lasttickpos.x) * CVarsUpdater::RenderPartialTick;
		b_box.max.y = pos.y + 1.8f - lpos.y + -pos.y + lasttickpos.y + (pos.y - lasttickpos.y) * CVarsUpdater::RenderPartialTick;
		b_box.max.z = pos.z + 0.3f - lpos.z + -pos.z + lasttickpos.z + (pos.z - lasttickpos.z) * CVarsUpdater::RenderPartialTick;

		bboxes.emplace_back(b_box);

		/*visual_entity.name = entity->Name;
		entity_list.push_back(visual_entity);*/
	}

	bboxxesdud = bboxes;
	//m_entity_list = entity_list;
	SLOW_SLEEP(1);
}

void CEsp::OnRender()
{
	if (!esp::enabled) return;

	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(CVarsUpdater::Projection.data());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(CVarsUpdater::ModelView.data());

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