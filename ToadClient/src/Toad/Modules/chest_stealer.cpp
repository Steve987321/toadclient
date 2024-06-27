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
	static int loot_item_counter = 0;
	static bool stopped = false;

	if (stopped)
	{
		if (m_isShiftDown)
		{
			send_key(VK_SHIFT, false);
			m_isShiftDown = false;
		}

		if (!GetAsyncKeyState(chest_stealer::steal_key))
			stopped = false;

		SLEEP(100);
		return;
	}

	static Timer aim_timer;
	static POINT current_pos{};

	if ((GetAsyncKeyState(chest_stealer::steal_key) & 0x8000 && CVarsUpdater::IsInGui) || looting /*(GetAsyncKeyState(VK_RBUTTON) & 1) && !GetAsyncKeyState(VK_SHIFT) && !chest_open*/)
	{		
		UpdateSlotPosOffsets();

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
			LOGDEBUG("[chest stealer] Looting {} items", m_indexPath.size() - 2);

			loot_item_counter = 0;

			// get the first index from the path
			index = m_indexPath.front();
			m_indexPath.pop_front();
			loot_item_counter++;

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
			if (loot_item_counter > 1 && ((GetAsyncKeyState(chest_stealer::steal_key) & 0x8000) || GetAsyncKeyState(0x45 /*E*/) & 0x8000 || GetAsyncKeyState(VK_ESCAPE) &0x8000))
			{
				loot_item_counter = 0;
				index = -1;
				looting = false;

				stopped = true;

				return;
			}

			const std::string& name = m_chestContents[index];
			//LOGDEBUG("target: {}", name);
			POINT pos = m_slotToMousePosOffset[index];
			const POINT middle = get_middle_of_screen();

			pos.x += middle.x;
			pos.y += middle.y;

			if (aim_timer.Elapsed() > chest_stealer::average_slowness_ms)
			{
				// make sure mouse is on slot and right click
				SetCursorPos(pos.x, pos.y);
				SLEEP(rand_int(50, 60));

				right_mouse_down(pos);
				SLEEP(rand_int(35, 70));
				right_mouse_up(pos);

				current_pos = pos;

				// get next in the path
				index = m_indexPath.front();
				m_indexPath.pop_front();
				loot_item_counter++;

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

		//LOGDEBUG("looting = true");
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

		if (!chest_stealer::items_to_grab.empty())
		{
			bool found = false;
			for (const auto& item : chest_stealer::items_to_grab)
			{
				if (name.find(item) != std::string::npos)
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				continue;
			}
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
		if (i.res_x == g_screen_width && i.res_y == g_screen_height)
		{
			info = &i;
			break;
		}

		if (i.res_x == -1 || i.res_y == -1)
		{
			info = &i;
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

		// imgui coordinates? 
		middle.x -= 12;
		middle.y -= 40;

		RECT desktop;
		GetWindowRect(GetDesktopWindow(), &desktop);
		int horizontal = (int)desktop.right;
		int vertical = (int)desktop.bottom;

		if (g_screen_height == vertical && g_screen_width == horizontal)
		{
			// imgui coordinates? 

			middle.y += 34;
			middle.x += 9;
		}

		for (int i = 0; i < 27; i++)
		{
			POINT pos = m_slotToMousePosOffset[i];

			pos.x += (int)middle.x;
			pos.y += (int)middle.y;
			
			const std::string& name = m_chestContents[i];
			const char* text = !name.empty() ? name.c_str() : std::to_string(i).c_str();
			draw->AddText({ (float)pos.x, (float)pos.y }, IM_COL32(255, 0, 0, 255), text);
		}
	}
}

}
