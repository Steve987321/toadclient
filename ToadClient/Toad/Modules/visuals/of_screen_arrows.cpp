#include "pch.h"
#include "Toad/toad.h"
#include "of_screen_arrows.h"

void toadll::COfScreenArrows::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	std::vector<std::pair<Entity, Vec3>> res;
	for (const auto& e : GetPlayerList())
	{
		/*double screenx, screeny, screenz;
		int viewport[4] = { CVarsUpdater::Viewport[0], CVarsUpdater::Viewport[1],CVarsUpdater::Viewport[2],CVarsUpdater::Viewport[3] };
		double modv[16] = {
			CVarsUpdater::ModelView[0], CVarsUpdater::ModelView[1], CVarsUpdater::ModelView[2], CVarsUpdater::ModelView[3],
			CVarsUpdater::ModelView[4], CVarsUpdater::ModelView[5], CVarsUpdater::ModelView[6], CVarsUpdater::ModelView[7],
			CVarsUpdater::ModelView[8], CVarsUpdater::ModelView[9], CVarsUpdater::ModelView[10], CVarsUpdater::ModelView[11],
			CVarsUpdater::ModelView[12], CVarsUpdater::ModelView[13], CVarsUpdater::ModelView[14], CVarsUpdater::ModelView[15]
		};
		double proj[16] = {
			CVarsUpdater::Projection[0], CVarsUpdater::Projection[1], CVarsUpdater::Projection[2], CVarsUpdater::Projection[3],
			CVarsUpdater::Projection[4], CVarsUpdater::Projection[5], CVarsUpdater::Projection[6], CVarsUpdater::Projection[7],
			CVarsUpdater::Projection[8], CVarsUpdater::Projection[9], CVarsUpdater::Projection[10], CVarsUpdater::Projection[11],
			CVarsUpdater::Projection[12], CVarsUpdater::Projection[13], CVarsUpdater::Projection[14], CVarsUpdater::Projection[15]
		};

		gluProject(
			e.Pos.x - lPlayer->Pos.x, -(e.Pos.y - lPlayer->Pos.y), e.Pos.z - lPlayer->Pos.z,
			modv, proj, viewport,
			&screenx, &screeny, &screenz);*/

		// check if off-screen
		//if (screenx < 0 || screenx > g_screen_width || screeny < 0 || screeny > g_screen_height)
			//res.push_back({ e, Vec3{static_cast<float>(screenx), static_cast<float>(screeny), static_cast<float>(screenz)} });

		Vec2 screen{};
		//WorldToScreen(e.Pos - lPlayer->Pos, screen, CVarsUpdater::ModelView, CVarsUpdater::Projection, g_screen_width, g_screen_height);
		//WorldToScreen(lPlayer->Pos, e.Pos,  CVarsUpdater::ModelView, CVarsUpdater::Projection, g_screen_width, g_screen_height);
		res.push_back({ e, Vec3{screen.x, screen.y, 1 } });

	}
	m_screenPositions = res;
	SLEEP(1);
}

void toadll::COfScreenArrows::OnImGuiRender(ImDrawList* draw)
{
	
}

void toadll::COfScreenArrows::OnRender()
{
	
}
