#include "pch.h"
#include "Toad/Toad.h"
#include "internal_ui.h"

#include "../Loader/src/utils/helpers.h"
#include "../Loader/src/utils/helpers.cpp"
#include "../Loader/src/utils/block_map.h"

#include "../Loader/src/Application/Fonts/Icons.h"
#include "../Loader/src/Application/ui.h"
#include "../Loader/src/Application/ui.cpp"

#include "nlohmann/json.hpp"
#include "../Loader/src/Application/config.h"
#include "../Loader/src/Application/config.cpp"

#include "imgui/imgui_internal.h"

using namespace toad;

namespace toadll
{
    void CInternalUI::OnImGuiRender(ImDrawList* draw)
    {
        //UIStyle();

        if (ui::show_array_list)
			ArrayList();

        if (!g_is_ui_internal)
            return;

        if (!MenuIsOpen)
            return;

        ImGuiIO* io = &ImGui::GetIO();

        static std::once_flag flag;
        std::call_once(flag, []
            {
                ImGui::SetNextWindowSize({500, 500});
            });

        ui::UI(io);
    }

    void CInternalUI::ArrayList()
    {
        auto enabledModules = GetEnabledModules();
        const auto font = HSwapBuffers::GetFont();

        std::ranges::sort(enabledModules, [&font](CModule* a, CModule* b)
        {
	        auto sizea = font->CalcTextSizeA(ui::array_list_size, 500, 0, a->name.c_str());
	        auto sizeb = font->CalcTextSizeA(ui::array_list_size, 500, 0, b->name.c_str());
	        return sizea.x > sizeb.x;
        });

        float sizeY = 0;
        for (const auto& m : enabledModules)
        {
            auto size = font->CalcTextSizeA(ui::array_list_size, 500, 0, m->name.c_str());
            size.x += 1;
            ImGui::GetForegroundDrawList()->AddText(font, ui::array_list_size, { g_screen_width - size.x, sizeY }, IM_COL32_WHITE, m->name.c_str());
            sizeY += size.y + 1;
            ImGui::GetForegroundDrawList()->AddRect({ g_screen_width - size.x - 2, sizeY - size.y }, { (float)g_screen_width + 2, sizeY + 1 }, IM_COL32(50, 50, 50, 255));
            ImGui::GetForegroundDrawList()->AddRectFilled({ g_screen_width - size.x - 1, sizeY - size.y }, {(float)g_screen_width + 1, sizeY}, IM_COL32(50, 50, 50, 150));
        } 
    }
}
