#pragma once

#include <functional>
#include <set>

#include "imgui/imgui.h"
#ifndef TOAD_LOADER
#include "../Loader/src/Application/Fonts/Icons.h"
#else
#include "Application/Fonts/Icons.h"
#endif
#include "imgui/imgui_internal.h"

// make enum work as flags
#define ENUM_FLAG(e) inline constexpr e operator | (e a, e b) { return static_cast<e>(static_cast<int>(a) | static_cast<int>(b)); }

// extra functions used throughout the project
namespace toad::utils
{
	struct window
	{
		window(const std::string_view title, DWORD pid, const HWND& hwnd) : title(title), pid(pid), hwnd(hwnd) {}
		std::string title;
		DWORD pid;
		HWND hwnd;
	};

	enum class mc_items
	{
		NONE = 0,
		HANDS = 1 << 1,
		SWORD = 1 << 2,
		AXE = 1 << 3,
		PICKAXE = 1 << 4,
		SHOVEL = 1 << 5,
		PLACEABLE = 1 << 6
	};
	ENUM_FLAG(mc_items);

	inline std::string to_string(mc_items item)
	{
		switch (item)
		{
		case mc_items::NONE: return "none";
		case mc_items::HANDS: return "hands";
		case mc_items::SWORD: return "sword";
		case mc_items::AXE: return "axe";
		case mc_items::PICKAXE: return "pickaxe";
		case mc_items::SHOVEL: return "shovel";
		case mc_items::PLACEABLE: return "blocks";
		}
		return "invalid";
	}

	enum class mboxType : short
	{
		NONE,
		WARNING,
		ERR,
		GOOD
	};

	// flags for imgui text placement
	enum center_text_flags
	{
		TEXT_LEFT = 1 << 1,
		TEXT_MIDDLEX = 1 << 2,
		TEXT_MIDDLEY = 1 << 3,
		TEXT_RIGHT = 1 << 4,
		TEXT_TOP = 1 << 5,
		TEXT_BOTTOM = 1 << 6,
		TEXT_MIDDLE = TEXT_MIDDLEX | TEXT_MIDDLEY
	};
	ENUM_FLAG(center_text_flags)

	// window scanner thread
	inline std::thread Twin_scan;
	void Fwin_scan();

	inline MC_CLIENT GetClientType(std::string_view window_title)
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

	template<typename T>
	std::vector<std::pair<std::string, T>> GetFilteredSuggestions(const std::string& input, const std::unordered_map<std::string, T>& map, int max_suggestions = 3)
	{
		if (input.size() <= 1) // only suggest when atleast 2 characters
			return {};

		std::vector<std::pair<std::string, T>> res = {};

		for (const auto& it : map)
		{
			if (it.first.find(input) != std::string::npos)
				res.emplace_back(it.first, it.second);

			if (res.size() > max_suggestions)
			{
				std::sort(res.begin(), res.end(), [](const auto& a, const auto& b) { return a.first < b.first;  });
				return res;
			}
		}

		std::sort(res.begin(), res.end(), [](const auto& a, const auto& b) { return a.first < b.first;  });
		return res;
	}

	template<typename T>
	std::vector<std::pair<T, std::string>> GetFilteredSuggestions(const std::string& input, const std::unordered_map<T, std::string>& map, const std::set<std::string> ignore = {}, int max_suggestions = 3)
	{
		if (input.size() <= 1) // only suggest when atleast 2 characters
			return {};
		std::vector<std::pair<T, std::string>> res = {};

		for (const auto& it : map)
		{
			if (it.second.find(input) != std::string::npos && !ignore.contains(it.second))
				res.emplace_back(it.first, it.second);

			if (res.size() >= max_suggestions)
			{
				std::sort(res.begin(), res.end(), [](const auto& a, const auto& b) { return a.second < b.second; });
				return res;
			}
		}

		std::sort(res.begin(), res.end(), [](const auto& a, const auto& b) { return a.second < b.second; });
		return res;
	}

	// only updated when still in init screen 
	inline std::vector<window> winListVec = {};

	/**
	 * \brief wrapper for getting the middle of the window
	 * \return the middle point of the current box or window
	 */
	inline ImVec2 get_middle_point()
	{
		return { ImGui::GetWindowSize().x / 2, ImGui::GetWindowSize().y / 2 };
	}

	// simple wrapper for centering a single line of text inside a box on the X axis
	template<typename ...Args>
	void center_textX(const ImVec4& col = {1,1,1,1}, const char* txt = "sample text", Args&& ...args)
	{
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2.f - ImGui::CalcTextSize(txt).x / 2.f);
		ImGui::TextColored(col, txt, args...);
	}

	// simple wrapper for centering a single line of text inside a box on the X and Y axis
	template<typename ...Args>
	void center_text(const ImVec4& col = {1,1,1,1}, const char* frmt = "sample text", Args&& ...args)
	{
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2.f - ImGui::CalcTextSize(frmt).x / 2.f);
		ImGui::SetCursorPosY(ImGui::GetWindowSize().y / 2.f - ImGui::CalcTextSize(frmt).y / 2.f);
		ImGui::TextColored(col, frmt, args...);
	}

	/**
	 * \brief simple wrapper for centering a multiline text inside a box on the X-axis
	 *
	 * \param txt string separated by '\n'
	 * \param col text color
	 * \param flags specify the position of the text
	 */
	inline void center_text_Multi(const ImVec4& col = { 1,1,1,1 }, std::string_view txt = "sample text \n second line", center_text_flags flags = TEXT_MIDDLE)
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

	/**
	 * \brief
	 * \param title 
	 * \param msg 
	 * \param condition 
	 * \param can_close 
	 * \param type 
	 * \param anim_speed 
	 */
	inline void show_mBox(const char* title, const char* msg, bool & condition, bool can_close = true, mboxType type = mboxType::NONE, float anim_speed = 12.f)
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

		ImGui::SetCursorPos({ get_middle_point().x - m_boxSize.x / 2, utils::get_middle_point().y - m_boxSize.y / 2 });
		ImGui::BeginChild(id.c_str(), m_boxSize, true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		{
			const auto titleMid = ImGui::GetWindowSize().x / 2.f - ImGui::CalcTextSize(title).x / 2.f;

			ImGui::PushStyleColor(ImGuiCol_Text, colText);
			ImGui::SetCursorPosX(titleMid);
			ImGui::Text(title);
			ImGui::PopStyleColor();
			//center_textX( colText, title);

			ImGui::SameLine();
			if (type == utils::mboxType::GOOD)
				ImGui::Text(ICON_FA_CHECK);
			else if (type == utils::mboxType::WARNING)
				ImGui::Text(ICON_FA_EXCLAMATION);
			else if (type == utils::mboxType::ERR)
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
			center_text_Multi(colText2, msg, TEXT_MIDDLEX);
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(2);

		ImGui::End();
		ImGui::PopStyleColor();
	}

	inline bool ToggleButton(const char* str_id, bool* v)
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

	inline bool checkboxButton(const char* name, const char* icon, bool* v)
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
		ToggleButton(id.c_str(), v);

		ImGui::SetCursorPosY(oPosY + 75 + style.FramePadding.y * 2);
		return res;
	}

	// use on modules
	inline void setting_menu(const char* name, bool& opened, const std::function<void()>& components, bool use_extra_options = false, const std::function<void()>& extra_options = {})
	{
		if (!opened) return;
		auto io = &ImGui::GetIO();
		//ImGuiContext& g = *GImGui;
		//const ImGuiStyle& style = g.Style;

		constexpr auto window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
		auto cbox_col = ImGui::GetColorU32(ImGuiCol_PopupBg);
		auto mid = get_middle_point();
		const ImVec2 box_size_real = { mid.x + 120, mid.y + 120 };
		static ImVec2 box_size_smooth = { mid.x + 15, mid.y + 15 };
		static float box_pos_X = get_middle_point().x - box_size_smooth.x / 2;
		static float box_pos_X_smooth = get_middle_point().x - box_size_smooth.x / 2;
		static float bg_alpha_smooth = 0;

		// extra options
		static bool options_open = false;
		static float options_bg_alpha_smooth = 0;
		static float options_bg_alpha_target = 0;
		static float options_box_pos_X = box_pos_X + box_size_smooth.x / 2 + 10;
		static float options_box_pos_X_smooth = box_pos_X + box_size_smooth.x / 2 + 10;

		const float options_box_size_X = box_size_smooth.x / 2 + 10;

		box_size_smooth = ImLerp(box_size_smooth, options_open ? ImVec2{mid.x + 100, mid.y + 120} : box_size_real, 20 * io->DeltaTime);
		bg_alpha_smooth = ImLerp(bg_alpha_smooth, 0.5f, 10 * io->DeltaTime);
		box_pos_X_smooth = ImLerp(box_pos_X_smooth, box_pos_X, 10 * io->DeltaTime);

		options_bg_alpha_smooth = ImLerp(options_bg_alpha_smooth, options_bg_alpha_target, 3 * io->DeltaTime);
		options_box_pos_X_smooth = ImLerp(options_box_pos_X_smooth, options_box_pos_X, 10 * io->DeltaTime);

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
		ImGui::SetCursorPos({ box_pos_X_smooth , get_middle_point().y - box_size_smooth.y / 2 });
		ImGui::BeginChild(name, box_size_smooth, true, window_flags);
		{
			center_textX({1,1,1,1}, name);
			
			ImGui::Separator();

			components();
		}
		ImGui::EndChild();

		if (use_extra_options)
		{
			if (options_open)
			{
				ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.1f, 0.1f, 0.1f, options_bg_alpha_smooth });
				ImGui::SetCursorPos({ options_box_pos_X_smooth, get_middle_point().y - box_size_smooth.y / 2 });
				ImGui::BeginChild("extra options", { box_size_smooth.x / 2 + 10, box_size_smooth.y }, true, window_flags);
				{
					center_textX({ 1,1,1,1 }, "Extra Options");

					ImGui::Separator();

					extra_options();
				}
				ImGui::EndChild();

				ImGui::PopStyleColor();
			}

			ImGui::SetCursorPos({ options_open ? get_middle_point().x + box_size_smooth.x / 3 : get_middle_point().x + box_size_smooth.x / 2, get_middle_point().y - box_size_smooth.y / 2 });

			ImGui::Text(ICON_FA_SLIDERS_H);
			ImGui::SetCursorPos({ options_open ? get_middle_point().x + box_size_smooth.x / 3 : get_middle_point().x + box_size_smooth.x / 2, get_middle_point().y - box_size_smooth.y / 2 });
			ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1,1,1,0.2f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 1,1,1,0.1f });
			ImGui::PushID(name);
			if (ImGui::Button("##", {20, 20}))
			{
				options_open = !options_open;

				options_bg_alpha_target = options_open ? 0.5f : 0.0f;
				options_box_pos_X = options_open ? get_middle_point().x + box_size_smooth.x / 4: box_pos_X + box_size_smooth.x / 2;
				box_pos_X = options_open ? 10 : get_middle_point().x - box_size_smooth.x / 2;

			}
			ImGui::PopID();
			ImGui::PopStyleColor(3);
			
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		// close when clicking off the settings menu
		const bool mbdown = ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right);
		if (mbdown && ImGui::IsWindowHovered())
		{
			box_size_smooth = { mid.x + 15, mid.y + 15 };
			opened = false;
		}

		ImGui::End();
		ImGui::PopStyleColor();
	}

	/**
	 * \brief wrapper for drawing a group box, Don't forget to call ImGui::EndChild();
	 * \param strID id
	 */
	inline void groupBox(const char* strID)
	{
		ImGui::BeginChild(strID, { 0,0 }, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings);
		center_textX({ 0.8f, 0.8f, 0.8f, 1 }, strID);
		ImGui::Separator();
	}

	// cool loading spinner
	inline bool load_spinner(const char* label, float radius, int thickness, const ImU32& color) {
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

}