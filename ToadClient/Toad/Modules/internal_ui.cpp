#include "pch.h"
#include "Toad/Toad.h"
#include "internal_ui.h"

#include "../Loader/src/utils/helpers.h"
#include "../Loader/src/utils/helpers.cpp"
#include "../Loader/src/utils/block_map.h"

#include "../Loader/src/Application/Fonts/Icons.h"
#include "../Loader/src/Application/ui.h"
#include "../Loader/src/Application/ui.cpp"

#include "imgui/imgui_internal.h"

using namespace toad;

namespace toadll
{
    void CInternalUI::OnImGuiRender(ImDrawList* draw)
    {
        //UIStyle();
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

}