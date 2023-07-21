#pragma once

#include <functional>
#include <set>

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#ifndef TOAD_LOADER
#include "../Loader/src/Application/Fonts/Icons.h"
#else
#include "Application/Fonts/Icons.h"
#endif

// make enum work as flags
#define ENUM_FLAG(e) inline constexpr e operator | (e a, e b) { return static_cast<e>(static_cast<int>(a) | static_cast<int>(b)); }

// extra functions used throughout the project
namespace toad
{

struct window
{
	window(const std::string_view title, DWORD pid, const HWND& hwnd)
		: title(title), pid(pid), hwnd(hwnd) {}

	std::string title;
	DWORD pid;
	HWND hwnd;
};

enum class MCItem
{
	NONE = 0,
	HANDS = 1 << 1,
	SWORD = 1 << 2,
	AXE = 1 << 3,
	PICKAXE = 1 << 4,
	SHOVEL = 1 << 5,
	PLACEABLE = 1 << 6
};
ENUM_FLAG(MCItem);

std::string to_string(MCItem item);

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

// only updated when still in init screen 
inline std::vector<window> winListVec = {};

// window scanner thread
inline std::thread window_scanner_thread;

void window_scanner();

MC_CLIENT GetClientType(std::string_view window_title);

template<typename T>
std::vector<std::pair<std::string, T>> get_filtered_suggestions(const std::string& input, const std::unordered_map<std::string, T>& map, int max_suggestions = 3)
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
std::vector<std::pair<T, std::string>> get_filtered_suggestions(const std::string& input, const std::unordered_map<T, std::string>& map, const std::set<std::string> ignore = {}, int max_suggestions = 3)
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

/// 
/// Wrapper for getting the middle of the window
///
ImVec2 get_middle_point();

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

///
/// Simple wrapper for centering a multiline text inside a box on the X-axis
///
/// @param txt string separated by '\n'
/// @param col text color
/// @param flags specify the position of the text
///
void center_text_multi(const ImVec4& col = { 1,1,1,1 }, std::string_view txt = "sample text \n second line", center_text_flags flags = TEXT_MIDDLE);

void show_message_box(const char* title, const char* msg, bool& condition, bool can_close = true, mboxType type = mboxType::NONE, float anim_speed = 12.f);

bool toggle_button(const char* str_id, bool* v);

bool checkbox_button(const char* name, const char* icon, bool* v);

// use on modules
void setting_menu(const char* name, bool& opened, const std::function<void()>& components, bool use_extra_options = false, const std::function<void()>& extra_options = {});

/// Wrapper for drawing a group box.
///
/// @param strID unique id of the box 
/// @note Don't forget to call ImGui::EndChild();
///
void group_box(const char* strID);

// cool loading spinner
bool load_spinner(const char* label, float radius, int thickness, const ImU32& color);
}