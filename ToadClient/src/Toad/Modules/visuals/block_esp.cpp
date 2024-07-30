#include "pch.h"
#include "Toad/toadll.h"
#include "block_esp.h"

#include "draw_helpers.h"

using namespace toad;

namespace toadll
{

CBlockEsp::CBlockEsp()
{
	Enabled = &block_esp::enabled;
}

void toadll::CBlockEsp::PreUpdate()
{
	WaitIsEnabled();
	WaitIsVerified();
}

void toadll::CBlockEsp::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	std::vector<std::pair<BBox, Vec4>> blockPositions = {};

	Vec3 lastTickPos = lPlayer->LastTickPos;
	Vec3 lPos = lastTickPos + (lPlayer->Pos - lastTickPos) * CVarsUpdater::RenderPartialTick;

	int block_x_limit = static_cast<int>(lPos.x) + m_range * 2;
	int block_y_limit = static_cast<int>(lPos.y) + m_range * 2;
	int block_z_limit = static_cast<int>(lPos.z) + m_range * 2;

	const jobject world = MC->getWorld();
	if (!world)
	{
		SLEEP(100);
		return;
	}

	static jclass blockAtClass = nullptr;
	for (int x = static_cast<int>(lPos.x) - m_range; x < block_x_limit; x++)
		for (int y = static_cast<int>(lPos.y) - m_range; y < block_y_limit; y++)
			for (int z = static_cast<int>(lPos.z) - m_range; z < block_z_limit; z++)
			{
				if (!CVarsUpdater::IsVerified)
					break;

				int id = MC->getBlockIdAt({ x,y,z });

				if (id == 0)
					continue; // airblock

				if (block_esp::block_list.contains(id))
				{
					blockPositions.emplace_back(
						BBox{
								Vec3
								{
									(float)x,
									(float)y,
									(float)z
								},

								Vec3
								{
									x + 1.0f,
									y + 1.0f,
									z + 1.0f
								}
						},
						Vec4{
							block_esp::block_list[id].x,
							block_esp::block_list[id].y,
							block_esp::block_list[id].z,
							block_esp::block_list[id].w,
						}
					);
				}
			}

	env->DeleteLocalRef(world);
	m_blocks = blockPositions;
	SLEEP(100);
}

void toadll::CBlockEsp::OnRender()
{
	if (!block_esp::enabled || !CVarsUpdater::IsVerified)
	{
		m_blocks.clear();
		return;
	}

	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(CVarsUpdater::Projection.data());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(CVarsUpdater::ModelView.data());

	glPushMatrix();
	glEnable(GL_LINE_SMOOTH);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDepthMask(false);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glLineWidth(1.f);

	Vec3 lPos = CVarsUpdater::theLocalPlayer->LastTickPos + (CVarsUpdater::theLocalPlayer->Pos - CVarsUpdater::theLocalPlayer->LastTickPos) * CVarsUpdater::RenderPartialTick;
	
	for (const auto& [block, col] : m_blocks)
		draw3d_bbox_fill(BBox{ block.min - lPos, block.max - lPos }, col);

	glDisable(GL_BLEND);
	glDepthMask(true);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LINE_SMOOTH);
	glPopMatrix();

	glPopMatrix();

}

}
