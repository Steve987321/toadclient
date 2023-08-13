#include "pch.h"
#include "Toad/Toad.h"
#include "right_autoclicker.h"

using namespace toad;

namespace toadll
{
	void CRightAutoClicker::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
	{
		static bool is_starting_click = false;

		if (!right_clicker::enabled)
		{
			SLEEP(250);
			return;
		}

		if (GetForegroundWindow() == g_hWnd && GetAsyncKeyState(VK_RBUTTON) && !CVarsUpdater::IsInGui)
		{
			m_start = std::chrono::high_resolution_clock::now();

			m_pTick = CVarsUpdater::PartialTick;

			if (!is_starting_click)
			{
				m_rand.edited_min = m_rand.min_delay;
				m_rand.edited_max = m_rand.max_delay;

				Timer start_delay_timer;
				while (start_delay_timer.Elapsed<>() < right_clicker::start_delayms)
				{
					if (!GetAsyncKeyState(VK_RBUTTON))
						return;
				}

				is_starting_click = true;
			}

			if (right_clicker::blocks_only)
				if (lPlayer->HeldItem.find("tile") == std::string::npos)
					return;

			if (!mouse_down())
				return;

			mouse_up();
		}
		else
		{
			if (is_starting_click)
			{
				for (auto& b : m_rand.boosts)
				{
					b.Reset();
				}
				is_starting_click = false;
			}

			SLEEP(50);
		}
	}

	Randomization& CRightAutoClicker::GetRand()
	{
		return m_rand;
	}

	void CRightAutoClicker::SetDelays(int cps)
	{
		m_rand.min_delay = (1000.f / (cps + 2)) / 2;
		m_rand.max_delay = (1000.f / (cps - 2)) / 2;
	}

	bool CRightAutoClicker::mouse_down()
	{
		m_rand.delay = rand_float(m_rand.edited_min, m_rand.edited_max);

		apply_rand(m_rand.inconsistencies);

		m_end = std::chrono::high_resolution_clock::now();
		m_delay_compensation = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_start).count()) / 1000.0f;

		precise_sleep((m_rand.delay + m_rand.inconsistency_delay - m_delay_compensation) / 1000.f);

		if (!GetAsyncKeyState(VK_RBUTTON))
			return false;

		POINT pt{};
		GetCursorPos(&pt);
		PostMessage(g_hWnd, WM_RBUTTONDOWN, MKF_RIGHTBUTTONDOWN, LPARAM((pt.x, pt.y)));

		m_start = std::chrono::high_resolution_clock::now();
		update_rand_vars();
		return true;
	}

	void CRightAutoClicker::mouse_up()
	{
		m_rand.delay = rand_float(m_rand.edited_min, m_rand.edited_max);

		apply_rand(m_rand.inconsistencies2);

		m_end = std::chrono::high_resolution_clock::now();
		m_delay_compensation = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_start).count()) / 1000.0f;

		precise_sleep((m_rand.delay + m_rand.inconsistency_delay - m_delay_compensation) / 1000.f);

		POINT pt{};
		GetCursorPos(&pt);
		PostMessage(g_hWnd, WM_RBUTTONUP, 0, LPARAM((pt.x, pt.y)));

		m_start = std::chrono::high_resolution_clock::now();
		update_rand_vars();
	}
}
