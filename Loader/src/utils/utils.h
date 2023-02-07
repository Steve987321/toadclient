#pragma once

#include "imgui/imgui.h"
#include "Application/Fonts/Icons.h"
#include "imgui/imgui_internal.h"

// make enum work as flags
#define ENUM_FLAG(e) inline constexpr e operator | (e a, e b) { return static_cast<e>(static_cast<int>(a) | static_cast<int>(b)); }

// extra functions
namespace toad::utils
{
	struct window
	{
		window(std::string_view title, DWORD pid, const HWND& hwnd) : title(title), pid(pid), hwnd(hwnd) {}
		std::string title;
		DWORD pid;
		HWND hwnd;
	};

	enum class mboxType
	{
		NONE,
		WARNING,
		ERR,
		GOOD
	};

	// flags for 
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

	// only updated when still in init screen !isverified
	inline std::vector<window> winListVec = {};


	/**
	 * \brief wrapper for getting the middle of the window
	 * \return the middle point of the current box or window
	 */
	inline ImVec2 get_middle_point()
	{
		return { ImGui::GetWindowSize().x / 2, ImGui::GetWindowSize().y / 2 };
	}

	// simple wrapper for centering a single line of text inside a box on the X-axis
	template<typename ...Args>
	void center_textX(const ImVec4& col = {1,1,1,1}, const char* txt = "sample text", Args&& ...args)
	{
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2.f - ImGui::CalcTextSize(txt).x / 2.f);
		ImGui::TextColored(col, txt, args...);
	}

	// simple wrapper for centering a single line of text inside a box on the X and Y-axis
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
			const auto& [lable, height]= lines[i];

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

	/**
	 * \brief wrapper for drawing begging a group box, Don't forget to call ImGui::EndChild();
	 * \param strID id
	 */
	inline void groupBox(const char* strID)
	{
		ImGui::BeginChild(strID, { 0,0 }, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings);
		center_textX({0.8f, 0.8f, 0.8f, 1}, strID);
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