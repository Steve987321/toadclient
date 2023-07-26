#include "pch.h"
#include "Toad/Toad.h"
#include "esp.h"

#include "draw_helpers.h"

using namespace toad;

namespace toadll {

void CEsp::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	if (!esp::enabled || !CVarsUpdater::IsVerified)
	{
		SLEEP(250);
		return;
	}


	std::lock_guard lock(m_bboxesMutex);

	// Update our bounding boxes list
	m_bboxes = GetBBoxes();
}

void CEsp::OnRender()
{
	if (!esp::enabled || !CVarsUpdater::IsVerified)
	{
		std::lock_guard lock(m_bboxesMutex);
		m_bboxes.clear();
		return;
	}

	if (m_bboxes.empty())
		return;

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

	m_bboxesMutex.lock();
	for (const auto& bb : m_bboxes)
	{
		draw2d_bbox(
			bb,
			{esp::fillCol[0], esp::fillCol[1], esp::fillCol[2], esp::fillCol[3]},
			{esp::lineCol[0], esp::lineCol[1], esp::lineCol[2], esp::lineCol[3]}
		);
	}
	m_bboxesMutex.unlock();

	glDisable(GL_BLEND);
	glDepthMask(true);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LINE_SMOOTH);
	glPopMatrix();

	glPopMatrix();
}

std::vector<BBox> CEsp::GetBBoxes() const
{
	std::vector<BBox> bboxesRes = {};

	for (const auto& entity : GetPlayerList())
	{
		if (entity.Invis)
			continue;

		auto player = MC->getLocalPlayer();

		auto playerlasttickpos = player->getLastTickPosition();
		auto currPos = player->getPosition();
		auto lpos = playerlasttickpos + (currPos - playerlasttickpos) * CVarsUpdater::RenderPartialTick;

		auto lasttickpos = entity.LastTickPos;
		auto pos = entity.Pos;

		BBox b_box = { {}, {} };
		b_box.min.x = pos.x - 0.3f - lpos.x + -pos.x + lasttickpos.x + (pos.x - lasttickpos.x) * CVarsUpdater::RenderPartialTick;
		b_box.min.y = pos.y - lpos.y + -pos.y + lasttickpos.y + (pos.y - lasttickpos.y) * CVarsUpdater::RenderPartialTick;
		b_box.min.z = pos.z - 0.3f - lpos.z + -pos.z + lasttickpos.z + (pos.z - lasttickpos.z) * CVarsUpdater::RenderPartialTick;
		b_box.max.x = pos.x + 0.3f - lpos.x + -pos.x + lasttickpos.x + (pos.x - lasttickpos.x) * CVarsUpdater::RenderPartialTick;
		b_box.max.y = pos.y + 1.8f - lpos.y + -pos.y + lasttickpos.y + (pos.y - lasttickpos.y) * CVarsUpdater::RenderPartialTick;
		b_box.max.z = pos.z + 0.3f - lpos.z + -pos.z + lasttickpos.z + (pos.z - lasttickpos.z) * CVarsUpdater::RenderPartialTick;

		//entityList.emplace_back(entity->Name, bbox{{-b_box.min.x, -b_box.min.y, -b_box.min.z}, {-b_box.max.x, -b_box.max.y, -b_box.max.z}});
		bboxesRes.emplace_back(b_box);

		/*visual_entity.name = entity->Name;
		entity_list.push_back(visual_entity);*/
	}

	return bboxesRes;
}

}
