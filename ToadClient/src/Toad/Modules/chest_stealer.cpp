#include "pch.h"
#include "Toad/Toad.h"
#include "chest_stealer.h"

using namespace toad;

namespace toadll
{

CChestStealer::CChestStealer()
{
	int index = 0;
	for (int j = chest_stealer::begin_y, sloty = 0; sloty < 3; j += 35, sloty++)
		for (int i = chest_stealer::begin_x, slotx = 0; slotx < 9; i += 35, slotx++)
		{
			m_slotToMousePosOffset[index++] = { i, j };
		}
}

void CChestStealer::PreUpdate()
{
	WaitIsVerified();
	CModule::PreUpdate();
}

void CChestStealer::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	Enabled = chest_stealer::enabled;

	if (!Enabled)
	{
		SLEEP(100);
		return;
	}

	static bool looting = false;
	static int index = -1;

	static Timer aim_timer;
	static POINT current_pos{};

	if (CVarsUpdater::IsInGui && GetAsyncKeyState(82) & 1 || looting /*(GetAsyncKeyState(VK_RBUTTON) & 1) && !GetAsyncKeyState(VK_SHIFT) && !chest_open*/)
	{		
		if (!looting)
		{
			{
				std::lock_guard lock(m_mutex);
				m_chestContents = MC->GetChestContents();

				if (m_chestContents.empty())
				{
					SLEEP(100);
					return;
				}

			}

			// setup mouse path
			SetupPath();

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
			looting = true;
		}

		if (index != -1)
		{
			if (GetAsyncKeyState(0x45 /*E*/) || GetAsyncKeyState(VK_ESCAPE))
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
				int x_lerp = (int)slerp(current_pos.x, pos.x, t);
				int y_lerp = (int)slerp(current_pos.y, pos.y, t);
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

		if (!chest_stealer::items_to_grab.empty() && std::ranges::find(chest_stealer::items_to_grab, name) == chest_stealer::items_to_grab.end())
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

//void CChestStealer::OnImGuiRender(ImDrawList* draw)
//{
//	if (chest_stealer::show_slot_positions)
//	{
//		ImVec2 middle = ImGui::GetMainViewport()->Size;
//		middle.x /= 2;
//		middle.y /= 2;
//		middle.x -= 10;
//		middle.y -= 50;
//
//		for (int i = 0; i < 27; i++)
//		{
//			POINT pos = m_slotToMousePosOffset[i];
//
//			pos.x += (int)middle.x;
//			pos.y += (int)middle.y;
//
//			draw->AddText({(float)pos.x, (float)pos.y}, IM_COL32(255, 0, 0, 255), std::to_string(i).c_str());
//		}
//	}
//
//	else if (chest_stealer::show_info)
//	{
//		ImVec2 middle = ImGui::GetMainViewport()->Size;
//
//		middle.x /= 2;
//		middle.y /= 2;
//		middle.x -= 10;
//		middle.y -= 50;
//
//		for (int i = 0; i < 27; i++)
//		{
//			const char* name;
//			{
//				std::lock_guard lock(m_mutex);
//				name = m_chestContents[i].data();
//			}
//
//			if (strlen(name) == 0)
//				continue;
//
//			POINT pos = m_slotToMousePosOffset[i];
//
//			pos.x += (int)middle.x;
//			pos.y += (int)middle.y;
//
//			draw->AddText({ (float)pos.x, (float)pos.y }, IM_COL32(255, 0, 0, 255), name);
//		}
//
//	}
//}

}
