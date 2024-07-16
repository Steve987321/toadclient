#include "pch.h"
#include "Toad/toadll.h"
#include "blink.h"

using namespace toad;

toadll::CBlink::CBlink()
{
	Enabled = &blink::enabled;
}

void toadll::CBlink::PreUpdate()
{
	WaitIsEnabled();
	WaitIsVerified();
	SLEEP(10);
}

void toadll::CBlink::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	if (!*Enabled)
	{
		SLEEP(250);
		return;
	}

	static std::vector<Vec3> tmpPositions = {};

	if (HWSASend::StopSends)
	{
		if (blink::show_trail)
		{
			static Timer savePosTimer;

			// save the current position in intervals of 200 ms
			if (!tmpPositions.empty() && savePosTimer.Elapsed<>() >= 200)
			{
				savePosTimer.Start();

				// don't need to save when player is standing still 
				if (tmpPositions.back() != lPlayer->Pos)
					tmpPositions.emplace_back(lPlayer->Pos);
			}
			else if (tmpPositions.empty())
			{
				tmpPositions.emplace_back(lPlayer->Pos);
			}

			if (m_can_save_position)
			{
				m_positions = tmpPositions;
			}
		}

		// check if blink has been enabled longer than the specified limit
		if (m_timer.Elapsed<>() >= blink::limit_seconds * 1000)
		{
			DisableBlink();
			m_can_enable = false;
		}
	}
	else
	{
		if (m_can_save_position)
		{
			m_positions.clear();
		}
		else
		{
			while (!m_can_save_position)
			{
				SLEEP(1);
			}

			m_positions.clear();
		}
		tmpPositions.clear();
		SLEEP(10);
	}

	if (!CVarsUpdater::IsInGui)
	{
		if (GetAsyncKeyState(blink::key) && m_can_enable)
		{
			HWSASend::StopSends = true;
			if (blink::stop_rec_packets)
				HWSARecv::StopRecvs = true;

			m_timer.Start();
			m_can_enable = false;
		}
		else if (!GetAsyncKeyState(blink::key))
		{
			DisableBlink();
			m_can_enable = true;
			SLEEP(10);
		}
	}

	SLEEP(1);
}

void toadll::CBlink::OnRender()
{
	if (!blink::show_trail || !blink::enabled || !HWSASend::StopSends || m_positions.empty())
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

	m_can_save_position = false;

	// draw lines connecting the positions when blinking
	for (unsigned int i = 1, j = 0; i < m_positions.size(); i++, j++)
	{
		const auto lPos = CVarsUpdater::theLocalPlayer->LastTickPos + (CVarsUpdater::theLocalPlayer->Pos - CVarsUpdater::theLocalPlayer->LastTickPos) * CVarsUpdater::RenderPartialTick;

		Vec3
		pos1 = m_positions[j] - lPos,
		pos2 = m_positions[i] - lPos;

		glBegin(GL_LINES);
		glVertex3f(pos1.x, pos1.y, pos1.z);
		glVertex3f(pos2.x, pos2.y, pos2.z);
		glEnd();
	}

	m_can_save_position = true;

	glDisable(GL_BLEND);
	glDepthMask(true);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LINE_SMOOTH);
	glPopMatrix();

	glPopMatrix();
}

void toadll::CBlink::DisableBlink()
{
	HWSASend::StopSends = false;
	if (HWSARecv::StopRecvs)
		HWSARecv::StopRecvs = false;
}
