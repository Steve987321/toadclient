#include "pch.h"
#include "Toad/Toad.h"
#include "blink.h"

using namespace toad;

void toadll::CBlink::DisableBlink()
{
	CWSASend::StopSends = false;
	if (c_WSARecv::StopRecvs)
		c_WSARecv::StopRecvs = false;
}

void toadll::CBlink::Update(const std::shared_ptr<LocalPlayerT>& lPlayer)
{
	if (!blink::enabled || !CVarsUpdater::IsVerified)
	{
		SLOW_SLEEP(100);
		return;
	}

	static bool canEnableFlag = true;
	static std::vector<vec3> tmp = {};

	if (CWSASend::StopSends)
	{
		if (blink::show_trail)
		{
			static CTimer savePosTimer;
			
			if (!tmp.empty() && savePosTimer.Elapsed<>() >= 200)
			{
				savePosTimer.Start();
				if (tmp.back() != lPlayer->Pos)
					tmp.emplace_back(lPlayer->Pos);
			}
			else if (tmp.empty())
			{
				tmp.emplace_back(lPlayer->Pos);
			}

			if (m_canSavePos)
			{
				m_positions = tmp;
			}
		}

		if (blink::disable_on_hit)
		{
			if (lPlayer->HurtTime > 0)
				DisableBlink();

			canEnableFlag = false;
		}

		if (m_timer.Elapsed<>() >= blink::limit_seconds * 1000)
		{
			DisableBlink();
			canEnableFlag = false;
		}
	}
	else
	{
		if (m_canSavePos)
		{
			m_positions.clear();
		}
		else
		{
			while (!m_canSavePos)
				SLOW_SLEEP(1);

			m_positions.clear();
		}
		tmp.clear();
	}

	if (!CVarsUpdater::IsInGui)
	{
		if (GetAsyncKeyState(blink::key) && canEnableFlag)
		{
			CWSASend::StopSends = true;
			if (blink::stop_rec_packets)
				c_WSARecv::StopRecvs = true;

			m_timer.Start();
			canEnableFlag = false;
		}
		else if (!GetAsyncKeyState(blink::key))
		{
			DisableBlink();
			canEnableFlag = true;
		}
	}
}

void toadll::CBlink::OnRender()
{
	if (!blink::show_trail || !blink::enabled || !CWSASend::StopSends || m_positions.empty())
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
	glLineWidth(3.f);

	m_canSavePos = false;

	for (unsigned int i = 1, j = 0; i < m_positions.size(); i++, j++)
	{
		const auto lPos = CVarsUpdater::LocalPlayer->LastTickPos + (CVarsUpdater::LocalPlayer->Pos - CVarsUpdater::LocalPlayer->LastTickPos) * CVarsUpdater::RenderPartialTick;

		vec3
		pos1 = m_positions[j] - lPos,
		pos2 = m_positions[i] - lPos;

		glBegin(GL_LINES);
		glVertex3f(pos1.x, pos1.y, pos1.z);
		glVertex3f(pos2.x, pos2.y, pos2.z);
		glEnd();
	}

	m_canSavePos = true;

	glDisable(GL_BLEND);
	glDepthMask(true);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LINE_SMOOTH);
	glPopMatrix();

	glPopMatrix();

}
