#pragma once

#include "imgui/imgui.h"
#include "Toad/Utils/file_dialog.h"
#include "Toad/Modules/VisualizeClicker/visualize_clicker.h"

namespace toad::UI
{
    inline VisualizeClicker visual_clicker;

    // loader extra setting window functions
    extern void clicker_rand_editor(bool* enabled);
    extern void clicker_rand_visualizer(bool* enabled);
    extern void esp_visualizer(bool* enabled);
    extern void chest_stealer_slotpos_setter(bool* enabled);

    // UI for toad when injected
    void ToadUI(const ImGuiIO* io);
}
