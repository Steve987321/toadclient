#include "toad.h"
#include "messageBox.h"

#include "imgui/imgui_internal.h"

namespace toad
{

void c_MessageBox::show()
{
	const std::string id = "##" + std::string(m_Title);

	static constexpr auto defCol = ImVec4(1.f,1.f,1.f,0.8f);

	static const ImVec2 m_boxSize = utils::get_middle_point();
	static ImVec4 m_Color = { 0,0,0,0 };

	std::cout << m_Color.x << " to " << defCol.x << std::endl;
	m_Color = ImLerp({0,0,0,0}, defCol, 5.f * ImGui::GetIO().DeltaTime);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 3.f);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, m_Color);

	ImGui::SetCursorPos({ utils::get_middle_point().x - m_boxSize.x / 2, utils::get_middle_point().y - m_boxSize.y / 2 });
	ImGui::BeginChild(id.c_str(), m_boxSize, true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoCollapse);
	{
		utils::center_textX({ 1,1,1,1 }, m_Title);

		ImGui::SameLine();
		if (m_Type == utils::mboxType::GOOD)
			ImGui::Text(ICON_FA_CHECK);
		else if (m_Type == utils::mboxType::WARNING)
			ImGui::Text(ICON_FA_EXCLAMATION);
		else if (m_Type == utils::mboxType::ERR)
			ImGui::Text(ICON_FA_CROSS);

		ImGui::Text("X");
		if (ImGui::IsItemClicked())
		{
			m_Condition = false;
		}
		ImGui::Separator();
		utils::center_text_Multi({ .8f,.8f,.8f,1 }, m_Msg);
	}
	ImGui::EndChild();

	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}

}
