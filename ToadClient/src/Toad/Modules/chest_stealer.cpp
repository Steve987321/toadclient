#include "pch.h"
#include "Toad/Toad.h"
#include "chest_stealer.h"

using namespace toad;

namespace toadll
{

CChestStealer::CChestStealer()
{
	Enabled = &chest_stealer::enabled;
	UpdateSlotPosOffsets();
}

void CChestStealer::PreUpdate()
{
	WaitIsEnabled();
	WaitIsVerified();
	CModule::PreUpdate();
}

void CChestStealer::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	if (!*Enabled)
	{
		SLEEP(100);
		return;
	}

	static bool looting = false;
	static int index = -1;

	static Timer aim_timer;
	static POINT current_pos{};

	if ((GetAsyncKeyState(chest_stealer::steal_key) & 0x8000 && CVarsUpdater::IsInGui) || looting /*(GetAsyncKeyState(VK_RBUTTON) & 1) && !GetAsyncKeyState(VK_SHIFT) && !chest_open*/)
	{		
		if (!looting)
		{
			{
				std::lock_guard lock(m_mutex);
				m_chestContents = MC->GetChestContents();

				if (m_chestContents.empty())
				{
					LOGDEBUG("[chest stealer] Empty chest");
					SLEEP(100);
					return;
				}

			}

			// setup mouse path
			SetupPath();
			LOGDEBUG("[chest stealer] Looting {} items", m_indexPath.size());

			// get the first index from the path
			index = m_indexPath.front();
			m_indexPath.pop_front();
		
			// start holding shift and wait a bit
			send_key(VK_SHIFT);
			m_isShiftDown = true;
			SLEEP(rand_int(30, 50));

			// start timer and set starting mouse position
			aim_timer.Start();
			GetCursorPos(&current_pos);
		}

		if (index != -1)
		{
			if (looting && (GetAsyncKeyState(chest_stealer::steal_key) & 0x8000 || GetAsyncKeyState(0x45 /*E*/) || GetAsyncKeyState(VK_ESCAPE)))
			{
				index = -1;
				return;
			}

			const std::string& name = m_chestContents[index];

			POINT pos = m_slotToMousePosOffset[index];
			const POINT middle = get_middle_of_screen();

			pos.x += middle.x;
			pos.y += middle.y;

			if (aim_timer.Elapsed() > chest_stealer::average_slowness_ms)
			{
				// make sure mouse is on slot and right click
				SetCursorPos(pos.x, pos.y);
				SLEEP(rand_int(30, 70));

				right_mouse_down(pos);
				SLEEP(rand_int(50, 100));
				right_mouse_up(pos);

				current_pos = pos;

				// get next in the path
				index = m_indexPath.front();
				m_indexPath.pop_front();

				// restart timer
				aim_timer.Start();
			}
			else
			{
				float t = std::clamp(aim_timer.Elapsed() / chest_stealer::average_slowness_ms, 0.f, 1.f);
				int x_lerp = (int)slerp((float)current_pos.x, (float)pos.x, t);
				int y_lerp = (int)slerp((float)current_pos.y, (float)pos.y, t);
				SetCursorPos(x_lerp, y_lerp);
			}
		}
		else
		{
			if (m_isShiftDown)
			{
				send_key(VK_SHIFT, false);
				m_isShiftDown = false;
			}

			looting = false;
		}

		// do it here so we don't check toggle key press at same time for disabling
		looting = true;
	}
	else
	{
		if (m_isShiftDown)
		{
			send_key(VK_SHIFT, false);
			m_isShiftDown = false;
		}
		looting = false;
	}

	if (index == -1)
	{
		if (m_isShiftDown)
		{
			send_key(VK_SHIFT, false);
			m_isShiftDown = false;
		}
		looting = false;
	}
}

void CChestStealer::SetupPath()
{
	m_indexPath.clear();

	for (int i = 0; i < m_chestContents.size(); i++)
	{
		const std::string& name = m_chestContents[i];
		if (name.empty())
		{
			continue;
		}

		if (!chest_stealer::items_to_grab.empty() && chest_stealer::items_to_grab.find(name) == std::string::npos)
		{
			continue;
		}

		m_indexPath.emplace_back(i);
	}

	// for now it just randomizes the path
	static auto rng = std::default_random_engine{};
	std::ranges::shuffle(m_indexPath, rng);

	m_indexPath.emplace_back(-1);
}

POINT CChestStealer::get_middle_of_screen() const
{
	RECT window_rect{};
	if (!GetWindowRect(g_hWnd, &window_rect))
	{
		return { -1, -1 };
	}

	POINT pt{};
	pt.x = window_rect.left + g_screen_width / 2;
	pt.y = window_rect.top + g_screen_height / 2;
	return pt;
}

void CChestStealer::right_mouse_down(const POINT& pt)
{
	PostMessage(g_hWnd, WM_RBUTTONDOWN, MKF_RIGHTBUTTONDOWN, LPARAM((pt.x, pt.y)));
}

void CChestStealer::right_mouse_up(const POINT& pt)
{
	PostMessage(g_hWnd, WM_RBUTTONUP, 0, LPARAM((pt.x, pt.y)));
}

void CChestStealer::UpdateSlotPosOffsets()
{
	int index = 0;
	
	ChestStealerSlotLocationInfo* info = nullptr;

	for (auto& i : chest_stealer::slot_info)
	{
		if (i.res_x == -1 || i.res_y == -1)
		{
			info = &i;
			break;
		}

		if (i.res_x == g_screen_width && i.res_y == g_screen_height)
		{
			info = &i;
			break;
		}
	}

	if (!info)
		return;

	for (int j = info->begin_y, sloty = 0; sloty < 3; j += info->space_y, sloty++)
		for (int i = info->begin_x, slotx = 0; slotx < 9; i += info->space_x, slotx++)
		{
			m_slotToMousePosOffset[index++] = { i, j };
		}
}

void CChestStealer::OnImGuiRender(ImDrawList* draw)
{
	if (CVarsUpdater::IsInGui && chest_stealer::show_slot_positions)
	{
		UpdateSlotPosOffsets();

		ImVec2 middle = ImGui::GetMainViewport()->Size;
		middle.x /= 2;
		middle.y /= 2;
		middle.x -= 10;
		middle.y -= 50;

		for (int i = 0; i < 27; i++)
		{
			POINT pos = m_slotToMousePosOffset[i];

			pos.x += (int)middle.x;
			pos.y += (int)middle.y;

			draw->AddText({ (float)pos.x, (float)pos.y }, IM_COL32(255, 0, 0, 255), std::to_string(i).c_str());
		}
	}
}

}
