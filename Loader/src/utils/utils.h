#pragma once

#include "imgui/imgui.h"

namespace toad::utils
{
	struct window
	{
		window(std::string_view title, DWORD pid, const HWND& hwnd) : title(title), pid(pid), hwnd(hwnd) {}
		std::string title;
		DWORD pid;
		HWND hwnd;
	};

	[[nodiscard]] std::vector<window> get_window_list(LPCSTR title);

	// window scanner thread
	inline std::thread twin_scan;
	void fwin_scan_thread();

	// only updated when still in init screen !isverified
	inline std::vector<window> winList = {};

	template<typename ...Args>
	void center_textX(const ImVec4& col = {1,1,1,1}, const char* txt = "sample text", Args&& ...args)
	{
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2.f - ImGui::CalcTextSize(txt).x / 2.f);
		ImGui::TextColored(col, txt, args...);
	}

	template<typename ...Args>
	void center_text(const ImVec4& col = {1,1,1,1}, const char* txt = "sample text", Args&& ...args)
	{
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2.f - ImGui::CalcTextSize(txt).x / 2.f);
		ImGui::SetCursorPosY(ImGui::GetWindowSize().y / 2.f - ImGui::CalcTextSize(txt).y / 2.f);
		ImGui::TextColored(col, txt, args...);
	}



}