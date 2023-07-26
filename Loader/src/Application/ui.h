#pragma once

namespace toad::ui
{
    void ui_main(const ImGuiIO* io);
    void ui_init(const ImGuiIO* io);

    // TODO: make it so that it is shared between toad loader and toadclient
    void UI(const ImGuiIO* io);
}