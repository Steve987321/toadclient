#include "toad.h"
#include "helpers.h"

namespace toad
{

bool ListProcessModules(DWORD dwPID)
{
    auto hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;

    // Take a snapshot of all modules in the specified process.
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
    if (hModuleSnap == INVALID_HANDLE_VALUE)
    {
       // std::cout << (TEXT("CreateToolhelp32Snapshot (of modules)"));
        return false;
    }

    // Set the size of the structure before using it.
    me32.dwSize = sizeof(MODULEENTRY32);

    // Retrieve information about the first module,
    // and exit if unsuccessful
    if (!Module32First(hModuleSnap, &me32))
    {
        std::cout << ("Module32First");  // show cause of failure
        CloseHandle(hModuleSnap);           // clean the snapshot object
        return false;
    }

    // Now walk the module list of the process,
    // and display information about each module
    do
    {
        //_tprintf(TEXT("\n\n     MODULE NAME:     %s"), me32.szModule);
        if (wcscmp(me32.szModule, L"jvm.dll") == 0 || wcscmp(me32.szModule, L"javaw.exe") == 0)
            return true;
       /* _tprintf(TEXT("\n     Executable     = %s"), me32.szExePath);
        _tprintf(TEXT("\n     Process ID     = 0x%08X"), me32.th32ProcessID);
        _tprintf(TEXT("\n     Ref count (g)  = 0x%04X"), me32.GlblcntUsage);
        _tprintf(TEXT("\n     Ref count (p)  = 0x%04X"), me32.ProccntUsage);
        _tprintf(TEXT("\n     Base address   = 0x%08X"), (DWORD)me32.modBaseAddr);
        _tprintf(TEXT("\n     Base size      = %d"), me32.modBaseSize);*/

    } while (Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);
    return false;
}

static BOOL CALLBACK enumWindowCallback(HWND hwnd, LPARAM lparam) {
    constexpr DWORD TITLE_SIZE = 1024;
    DWORD PID = 0;

    WCHAR windowTitle[TITLE_SIZE];

    GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);

    const int length = ::GetWindowTextLength(hwnd);

    if (IsWindowVisible(hwnd) && length != 0) {
        //convert to std::string
        auto buf = new char[length + 1];

        char DefChar = ' ';
        WideCharToMultiByte(CP_ACP, 0, windowTitle, -1, buf, length + 1, &DefChar, NULL);

        auto title = std::string(buf);
        std::ranges::transform(title, title.begin(), tolower);

        if (title.find("lunar client") != std::string::npos || title.find("minecraft") != std::string::npos)
        {
            GetWindowThreadProcessId(hwnd, &PID);
            if (ListProcessModules(PID))
				winListVec.emplace_back(title, PID, hwnd);
        }

        delete[] buf;

        return TRUE;
    }
    return TRUE;
}

void window_scanner()
{
    while (g_is_running)
    {
        if (!g_is_verified)
        {
            winListVec.clear();
            EnumWindows(enumWindowCallback, 0);
        }
        SLEEP(1000);
    }
}

MC_CLIENT GetClientType(std::string_view window_title)
{
	std::cout << window_title << std::endl;
	if (window_title.find("lunar client") == std::string::npos)
		return MC_CLIENT::NOT_SUPPORTED;
	if (window_title.find("1.8.9") != std::string::npos)
		return MC_CLIENT::Lunar_189;
	if (window_title.find("1.7.10") != std::string::npos)
		return MC_CLIENT::Lunar_171;

	return MC_CLIENT::NOT_SUPPORTED;
}

ImVec2 get_middle_point()
{
	return { ImGui::GetWindowSize().x / 2, ImGui::GetWindowSize().y / 2 };
}

void center_text_multi(const ImVec4& col, std::string_view txt, center_text_flags flags)
{
	std::string str;
	std::vector <std::pair<std::string, float>> lines = {};
	float txtHeight = 0;

	for (size_t i = 0 ; i < txt.length(); i++)
	{
		const char c = txt[i];
		str += c;
		if (c == '\n' || i == txt.length() - 1)
		{
			auto height = ImGui::CalcTextSize(str.c_str()).y;
			txtHeight += height;
			lines.emplace_back(str.c_str(), height);
			str.clear();
		}
	}

	// render our multi line text
	for (int i = 0; i < lines.size(); i++)
	{
		const auto& [lable, height] = lines[i];

		assert("TEXT_MIDDLEX conflicts with TEXT_LEFT and TEXT_RIGHT " && !(flags & TEXT_MIDDLEX && (flags & TEXT_LEFT || flags & TEXT_RIGHT)));
		assert("TEXT_MIDDLEY conflicts with TEXT_TOP and TEXT_BOTTOM " && !(flags & TEXT_MIDDLEY && (flags & TEXT_TOP || flags & TEXT_BOTTOM)));

		if (flags & TEXT_MIDDLEY)
		{
			auto yDist = ImGui::GetStyle().ItemSpacing.y;
			auto yPosNew = ImGui::GetWindowSize().y / 2.f - height / 2.f;
			auto yPos = ImGui::GetCursorPosY();

			if (i == 0)
				ImGui::SetCursorPosY(yPosNew - txtHeight);

			if (yPos == yPosNew)
				ImGui::SetCursorPosY(yPosNew + yDist);
		}
		if (flags & TEXT_MIDDLEX)
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2.f - ImGui::CalcTextSize(lable.c_str()).x / 2.f);

		ImGui::TextColored(col, lable.c_str());
	}
}

void show_message_box(const char* title, const char* msg, bool& condition, bool can_close, mboxType type, float anim_speed)
{
	auto io = &ImGui::GetIO();

#ifdef IMGUI_HAS_VIEWPORT
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->GetWorkPos());
	ImGui::SetNextWindowSize(viewport->GetWorkSize());
	ImGui::SetNextWindowViewport(viewport->ID);
#else
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(io->DisplaySize);
#endif
	static float wBgAlpha = 0;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.1f,0.1f,0.1f, wBgAlpha });
	ImGui::Begin("overlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

	auto mid = get_middle_point();
	const std::string id = "##" + std::string(title);

	static constexpr auto defCol = ImVec4(0.1f, 0.1f, 0.1f, 1.f);

	const auto defBorderCol = ImGui::GetStyleColorVec4(ImGuiCol_Border);

	static ImVec2 m_boxSize = { mid.x + 15, mid.y + 15};

	static ImVec4 col = { 0,0,0,0 };
	static ImVec4 colText = { 0,0,0,0 };
	static ImVec4 colText2 = { 0,0,0,0 };
	static ImVec4 colBorder = { 0,0,0,0 };
	
	col = ImLerp(col, defCol, anim_speed * io->DeltaTime);
	colText = ImLerp(colText, {1,1,1,1}, anim_speed * io->DeltaTime);
	colText2 = ImLerp(colText2, { .8f,.8f,.8f,1 }, anim_speed * io->DeltaTime);
	colBorder = ImLerp(colBorder, defBorderCol, anim_speed * io->DeltaTime);
	wBgAlpha = ImLerp(wBgAlpha, 0.5f, anim_speed * io->DeltaTime);
	m_boxSize = ImLerp(m_boxSize, get_middle_point(), (anim_speed * 1.5f) * io->DeltaTime);

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.f);
	ImGui::PushStyleColor(ImGuiCol_Border, colBorder);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, col);

	ImGui::SetCursorPos({ get_middle_point().x - m_boxSize.x / 2, get_middle_point().y - m_boxSize.y / 2 });
	ImGui::BeginChild(id.c_str(), m_boxSize, true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	{
		const auto titleMid = ImGui::GetWindowSize().x / 2.f - ImGui::CalcTextSize(title).x / 2.f;

		ImGui::PushStyleColor(ImGuiCol_Text, colText);
		ImGui::SetCursorPosX(titleMid);
		ImGui::Text(title);
		ImGui::PopStyleColor();
		//center_textX( colText, title);

		ImGui::SameLine();
		if (type == mboxType::GOOD)
			ImGui::Text(ICON_FA_CHECK);
		else if (type == mboxType::WARNING)
			ImGui::Text(ICON_FA_EXCLAMATION);
		else if (type == mboxType::ERR)
			ImGui::Text(ICON_FA_CROSS);
		else
			ImGui::Text("");

		if (can_close)
		{
			ImGui::SameLine();
			if (ImGui::Button("X"))
			{
				col = { 0,0,0,0 };
				colBorder = { 0,0,0,0 };
				colText = { 0,0,0,0 };
				colText2 = { 0,0,0,0 };
				m_boxSize = { mid.x + 15, mid.y + 15 };
				wBgAlpha = 0;
				condition = false;
			}
		}
		ImGui::Separator();
		center_text_multi(colText2, msg, TEXT_MIDDLEX);
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor(2);

	ImGui::End();
	ImGui::PopStyleColor();
}

bool toggle_button(const char* str_id, bool* v)
{
	//ImVec2 p = ImGui::GetCursorScreenPos();
	//p.x += 12; // +12 for button size change
	//ImDrawList* draw_list = ImGui::GetWindowDrawList();

	//float height = ImGui::GetFrameHeight();
	//float width = height * 1.55f;
	//float radius = height * 0.50f;

	//if (ImGui::InvisibleButton(str_id, ImVec2(width, height)))
	//	*v = !*v;
	//ImU32 col_bg;

	//bool res = ImGui::IsItemHovered();

	//if (res)
	//	col_bg = *v ? IM_COL32(145 + 20, 211, 68 + 20, 255) : IM_COL32(218 - 20, 218 - 20, 218 - 20, 255);
	//else
	//	col_bg = *v ? IM_COL32(145, 211, 68, 255) : IM_COL32(218, 218, 218, 255);

	//draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
	//draw_list->AddCircleFilled(ImVec2(*v ? (p.x + width - radius) : (p.x + radius), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
	//return res;

	// new "toggle button"

	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float height = ImGui::GetFrameHeight();
	float width = height * 1.55f;

	if (ImGui::InvisibleButton(str_id, ImVec2(width, height)))
		*v = !*v;
	ImU32 col_bg;

	bool res = ImGui::IsItemHovered();

	if (res)
		col_bg = *v ? IM_COL32(165, 211, 88, 255) : IM_COL32(231, 68, 91, 255);
	else
		col_bg = *v ? IM_COL32(145, 211, 68, 255) /* #91D344 */ : IM_COL32(211, 68, 71, 255); /* #D34447 */

	// horizontal
	draw_list->AddRectFilled(p, ImVec2(p.x + 104, p.y + 5), col_bg);
	draw_list->AddRectFilled(ImVec2(p.x, p.y + 70), ImVec2(p.x + 104, p.y + 75), col_bg);

	// vertical
	draw_list->AddRectFilled(ImVec2(p.x, p.y + 5), ImVec2(p.x + 5, p.y + 70), col_bg);
	draw_list->AddRectFilled(ImVec2(p.x + 99, p.y + 5), ImVec2(p.x + 104, p.y + 70), col_bg);

	return res;
}

bool checkbox_button(const char* name, const char* icon, bool* v)
{
	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	const auto id = "##" + std::string(name);
	const auto oPosY = ImGui::GetCursorPosY();
	const auto oPosX = ImGui::GetCursorPosX();

	bool res = false;

	ImGui::Button(id.c_str(), { 104, 75 });
	
	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			*v = !*v;
		}
		else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			res = true;
		}
	}
	ImGui::SetCursorPosY(oPosY + style.FramePadding.y);
	ImGui::SetCursorPosX(oPosX + 25);
	//ToggleButton(id.c_str(), v);
	ImGui::SetCursorPos({ oPosX + 52 - ImGui::CalcTextSize(icon).x / 2, oPosY + 28 });
	ImGui::Text(icon);
	ImGui::SetCursorPos({ oPosX + 52 - ImGui::CalcTextSize(name).x / 2, oPosY + 48 });
	ImGui::Text(name);

	// green/red thing
	ImGui::SetCursorPos({ oPosX, oPosY });
	toggle_button(id.c_str(), v);

	ImGui::SetCursorPosY(oPosY + 75 + style.FramePadding.y * 2);
	return res;
}

void setting_menu(const char* name, bool& opened, const std::function<void()>& components, bool use_extra_options, const std::function<void()>& extra_options)
{
	if (!opened) return;

	auto io = &ImGui::GetIO();

	constexpr auto window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

	auto cbox_col = ImGui::GetColorU32(ImGuiCol_PopupBg);
	auto mid = get_middle_point();
	const ImVec2 kbox_size_real = { mid.x + 120, mid.y + 120 };
	static ImVec2 box_size_smooth = { mid.x + 15, mid.y + 15 };
	const static float kbox_pos_X = mid.x - box_size_smooth.x / 2;
	static float box_pos_X = kbox_pos_X;
	static float box_pos_X_smooth = kbox_pos_X;
	static float bg_alpha_smooth = 0;

	// extra options
	static bool is_eoptions_open = false;
	static float eoptions_bg_alpha_smooth = 0;
	static float eoptions_bg_alpha_target = 0;
	static float eoptions_box_pos_X = box_pos_X + box_size_smooth.x - 10;
	static float eoptions_box_pos_X_smooth = box_pos_X + box_size_smooth.x - 10;

	// update box sizes and positions using anim_speed
	const auto anim_speed = std::clamp(20.f * io->DeltaTime, 0.f, 1.f);

	box_size_smooth = ImLerp(box_size_smooth, is_eoptions_open ? ImVec2{mid.x + 100, mid.y + 120} : kbox_size_real, anim_speed);
	bg_alpha_smooth = ImLerp(bg_alpha_smooth, 0.5f, anim_speed);
	box_pos_X_smooth = ImLerp(box_pos_X_smooth, box_pos_X, anim_speed);

	eoptions_bg_alpha_smooth = ImLerp(eoptions_bg_alpha_smooth, eoptions_bg_alpha_target, anim_speed);
	eoptions_box_pos_X_smooth = ImLerp(eoptions_box_pos_X_smooth, eoptions_box_pos_X, anim_speed);

	// darker bg

#ifdef IMGUI_HAS_VIEWPORT
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->GetWorkPos());
	ImGui::SetNextWindowSize(viewport->GetWorkSize());
	ImGui::SetNextWindowViewport(viewport->ID);
#else
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(io->DisplaySize);
#endif

	ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.1f,0.1f,0.1f, bg_alpha_smooth });
	ImGui::Begin("overlay", nullptr, window_flags);

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.f);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, cbox_col);
	ImGui::SetCursorPos({ box_pos_X_smooth , mid.y - box_size_smooth.y / 2 });
	ImGui::BeginChild(name, box_size_smooth, true, window_flags);
	{
		center_textX({1,1,1,1}, name);
		
		ImGui::Separator();

		components();
	}
	ImGui::EndChild();

	ImGui::PopStyleVar();
	ImGui::PopStyleColor();

	if (use_extra_options)
	{
		if (is_eoptions_open)
		{
			ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.1f, 0.1f, 0.1f, eoptions_bg_alpha_smooth });
			ImGui::SetCursorPos({ eoptions_box_pos_X_smooth, get_middle_point().y - box_size_smooth.y / 2 });
			ImGui::BeginChild("extra options", { box_size_smooth.x / 2 + 10, box_size_smooth.y }, true, window_flags);
			{
				center_textX({ 1,1,1,1 }, "Extra Options");

				ImGui::Separator();

				extra_options();
			}
			ImGui::EndChild();

			ImGui::PopStyleColor();
		}

		const ImVec2 eoptions_button_pos = {
			box_pos_X_smooth + box_size_smooth.x,
			get_middle_point().y - box_size_smooth.y / 2 
		};

		ImGui::SetCursorPos(eoptions_button_pos);
		ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1,1,1,0.2f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 1,1,1,0.1f });
		ImGui::PushID(name);
		if (ImGui::Button("##", {22, box_size_smooth.y}))
		{
			is_eoptions_open = !is_eoptions_open;

			eoptions_bg_alpha_target = is_eoptions_open ? 0.5f : 0.0f;
			eoptions_box_pos_X = is_eoptions_open ? get_middle_point().x + box_size_smooth.x / 4: box_pos_X + box_size_smooth.x - 10;
			box_pos_X = is_eoptions_open ? 10 : get_middle_point().x - box_size_smooth.x / 2;
		}
		ImGui::PopID();
		ImGui::PopStyleColor(3);
		
	}

	// close when clicking off the settings menu
	const bool mbdown = ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right);
	if (mbdown && ImGui::IsWindowHovered())
	{
		box_size_smooth = { mid.x + 15, mid.y + 15 };
		is_eoptions_open = false;
		eoptions_bg_alpha_target = 0.0f;
		eoptions_box_pos_X = box_pos_X + box_size_smooth.x / 2;
		box_pos_X = kbox_pos_X;
		opened = false;
	}

	ImGui::End();
	ImGui::PopStyleColor();
}

void group_box(const char* strID)
{
	ImGui::BeginChild(strID, { 0,0 }, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings);
	center_textX({ 0.8f, 0.8f, 0.8f, 1 }, strID);
	ImGui::Separator();
}

bool load_spinner(const char* label, float radius, int thickness, const ImU32& color)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ImGui::ItemSize(bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	// Render
	auto draw = ImGui::GetForegroundDrawList();
	draw->PathClear();// window->DrawList->PathClear();

	int num_segments = 30;
	int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

	const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
	const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

	const auto centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

	for (int i = 0; i < num_segments; i++) {
		const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
		draw->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
		                        centre.y + ImSin(a + g.Time * 8) * radius));
	}

	draw->PathStroke(color, false, thickness);
	return true;
}

std::string to_string(MCItem item)
{
    switch (item)
    {
    case MCItem::NONE: return "none";
    case MCItem::HANDS: return "hands";
    case MCItem::SWORD: return "sword";
    case MCItem::AXE: return "axe";
    case MCItem::PICKAXE: return "pickaxe";
    case MCItem::SHOVEL: return "shovel";
    case MCItem::PLACEABLE: return "blocks";
    }
    return "invalid";
}

}
