#include "pch.h"
#include "Toad/Toad.h"
#include "esp.h"

using namespace toad;

namespace toadll {

std::vector<BBox> CEsp::GetBBoxes(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	std::vector<BBox> bboxesRes = {};

	for (const auto& entity : CVarsUpdater::PlayerList)
	{
		if (entity.Invis)
			continue;

		auto playerlasttickpos = lPlayer->LastTickPos;
		auto currPos = lPlayer->Pos;
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

void CEsp::draw3dBox(const BBox& bbox)
{
	glBegin(3);
	glColor4f(toad::esp::lineCol[0], toad::esp::lineCol[1], toad::esp::lineCol[2], toad::esp::lineCol[3]);
	glVertex3f(bbox.min.x, bbox.min.y, bbox.min.z);
	glVertex3f(bbox.max.x, bbox.min.y, bbox.min.z);
	glVertex3f(bbox.max.x, bbox.min.y, bbox.max.z);
	glVertex3f(bbox.min.x, bbox.min.y, bbox.max.z);
	glVertex3f(bbox.min.x, bbox.min.y, bbox.min.z);
	glEnd();

	glBegin(3);
	glColor4f(toad::esp::lineCol[0], toad::esp::lineCol[1], toad::esp::lineCol[2], toad::esp::lineCol[3]);
	glVertex3f(bbox.min.x, bbox.max.y, bbox.min.z);
	glVertex3f(bbox.max.x, bbox.max.y, bbox.min.z);
	glVertex3f(bbox.max.x, bbox.max.y, bbox.max.z);
	glVertex3f(bbox.min.x, bbox.max.y, bbox.max.z);
	glVertex3f(bbox.min.x, bbox.max.y, bbox.min.z);
	glEnd();

	glBegin(1);
	glColor4f(toad::esp::lineCol[0], toad::esp::lineCol[1], toad::esp::lineCol[2], toad::esp::lineCol[3]);
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

void CEsp::draw2dBox(const BBox& bbox)
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
	glColor4f(toad::esp::fillCol[0], toad::esp::fillCol[1], toad::esp::fillCol[2], toad::esp::fillCol[3]);
	glBegin(GL_QUADS);
	for (const auto& vertice : vertices)
	{
		glVertex3f(vertice.x, vertice.y, vertice.z);
	}
	glEnd();

	// outlines
	glColor4f(toad::esp::lineCol[0], toad::esp::lineCol[1], toad::esp::lineCol[2], toad::esp::lineCol[3]);
	glBegin(GL_LINE_LOOP);
	for (const auto& vertice : vertices)
	{
		glVertex3f(vertice.x, vertice.y, vertice.z);
	}
	glEnd();
}

void CEsp::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	if (!esp::enabled || !CVarsUpdater::IsVerified)
	{
		SLEEP(250);
		return;
	}

	// Update our bounding boxes list
	m_bboxes = GetBBoxes(lPlayer);
}

void CEsp::OnRender()
{
	if (!esp::enabled || !CVarsUpdater::IsVerified) 
	{
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

	for (const auto& bb : m_bboxes)
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
