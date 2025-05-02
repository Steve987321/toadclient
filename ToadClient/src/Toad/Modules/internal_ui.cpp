#include "pch.h"
#include "Toad/toad.h"
#include "internal_ui.h"

#include "Toad/utils/helpers.h"
#include "Toad/Utils/block_map.h"

#include "Toad/Fonts/icons.h"
#include "Toad/ToadUI.h"

#include "nlohmann/json.hpp"

#include "imgui/imgui_internal.h"

namespace toad
{
    void CInternalUI::OnImGuiRender(ImDrawList* draw)
    {
        //UIStyle();

        if (settings.ui_show_array_list)
			ArrayList();

        if (!settings.g_is_ui_internal)
            return;

        if (!MenuIsOpen)
            return;

        ImGuiIO* io = &ImGui::GetIO();

        static std::once_flag flag;
        std::call_once(flag, []
            {
                ImGui::SetNextWindowSize({500, 500});
            });

        UI::ToadUI(io);
    }

    void CInternalUI::ArrayList()
    {
        const ImFont* font = HSwapBuffers::GetFont();

        std::vector<const CModule*> enabled_modules;
        for (const CModule* m : CModule::ModuleInstances)
        {
            if (m->Enabled)
                enabled_modules.emplace_back(m);
        }

		std::sort(enabled_modules.begin(), enabled_modules.end(), [&font](const CModule* a, const CModule* b)
			{
				auto sizea = font->CalcTextSizeA((float)settings.ui_array_list_size, 500, 0, a->Name.c_str());
				auto sizeb = font->CalcTextSizeA((float)settings.ui_array_list_size, 500, 0, b->Name.c_str());
				return sizea.x > sizeb.x;
			});

        float sizeY = 0;
        for (const auto& m : enabled_modules)
        {
            auto size = font->CalcTextSizeA((float)settings.ui_array_list_size, 500, 0, m->Name.c_str());
            size.x += 1;
            ImGui::GetForegroundDrawList()->AddText(font, (float)settings.ui_array_list_size, { g_screen_width - size.x, sizeY }, IM_COL32_WHITE, m->Name.c_str());
            sizeY += size.y + 1;
            ImGui::GetForegroundDrawList()->AddRect({ g_screen_width - size.x - 2, sizeY - size.y }, { (float)g_screen_width + 2, sizeY + 1 }, IM_COL32(50, 50, 50, 255));
            ImGui::GetForegroundDrawList()->AddRectFilled({ g_screen_width - size.x - 1, sizeY - size.y }, {(float)g_screen_width + 1, sizeY}, IM_COL32(50, 50, 50, 150));
        } 
    }
}
