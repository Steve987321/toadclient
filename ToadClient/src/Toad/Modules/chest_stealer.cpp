#include "pch.h"
#include "Toad/Toad.h"
#include "chest_stealer.h"

using namespace toad;

namespace toadll
{

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

	static bool chest_open = false;

	if (GetAsyncKeyState(82) & 1 /*(GetAsyncKeyState(VK_RBUTTON) & 1) && !GetAsyncKeyState(VK_SHIFT) && !chest_open*/)
	{
		std::array<std::string, 27> contents = MC->GetChestContents();

		if (contents.empty())
		{
			SLEEP(100);
			return;
		}

		send_key(VK_SHIFT);

		SLEEP(100);

		const POINT middle = get_middle_of_screen();
		for (int i = 0; i < 27; i++)
		{
			const std::string& name = contents[i];
			if (name.empty())
				continue;

			POINT pos = m_slotToMousePosOffset[i];

			pos.x += middle.x;
			pos.y += middle.y;

			
			SetCursorPos(pos.x, pos.y);
			SLEEP(50);
			right_mouse_down(pos);
			SLEEP(10);
			right_mouse_up(pos);
			SLEEP(300);
		}

		send_key(VK_SHIFT, false);

		chest_open = true;
	}

	if (chest_open && (GetAsyncKeyState(VK_ESCAPE) || GetAsyncKeyState(0x45 /*E*/)))
	{
		chest_open = false;
	}

	get_middle_of_screen();
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

CChestStealer::CChestStealer()
{
	int index = 0;
	for (int j = chest_stealer::begin_y, sloty = 0; sloty < 3; j += 35, sloty++)
		for (int i = chest_stealer::begin_x, slotx = 0; slotx < 9; i += 35, slotx++)
		{
			LOGDEBUG("i:{} \t j:{} \t index:{}", i, j, index);
			m_slotToMousePosOffset[index++] = { i, j };
		}
}

void CChestStealer::right_mouse_down(const POINT& pt)
{
	PostMessage(g_hWnd, WM_RBUTTONDOWN, MKF_RIGHTBUTTONDOWN, LPARAM((pt.x, pt.y)));
}

void CChestStealer::right_mouse_up(const POINT& pt)
{
	PostMessage(g_hWnd, WM_RBUTTONUP, 0, LPARAM((pt.x, pt.y)));
}

void CChestStealer::OnImGuiRender(ImDrawList* draw)
{
	if (chest_stealer::show_grid)
	{
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

			draw->AddText({(float)pos.x, (float)pos.y}, IM_COL32(255, 0, 0, 255), std::to_string(i).c_str());
		}
	}
}

}
