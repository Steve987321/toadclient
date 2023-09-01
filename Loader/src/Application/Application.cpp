#include "toad.h"

#include "imgui/imgui.h"

#include <d3d9.h>
#include <d3d9types.h>
#pragma comment(lib, "d3d9.lib")

#include "imgui/backends/imgui_impl_dx9.h"
#include "imgui/backends/imgui_impl_win32.h"

#include "Application.h"

#include "Fonts/fa-solid-900Font.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Data
static LPDIRECT3D9              g_pD3D;
static LPDIRECT3DDEVICE9        g_pd3dDevice;
static D3DPRESENT_PARAMETERS    g_d3dpp;

namespace toad
{
    Application::Application()
    {
        // Auto load configs
        loader_path = std::filesystem::current_path().string();
        auto configs = config::GetAllConfigsInDirectory(loader_path);
        if (!configs.empty())
        {
            // load the only available config 
            if (configs.size() == 1)
            {
                config::LoadConfig(loader_path, configs.begin()->FileNameStem);
            }
            else
            {
                auto it = std::ranges::min_element(configs, [](const config::ConfigFile& a, const config::ConfigFile& b) {return a.LastWrite > b.LastWrite; });

                if (it != configs.end())
                {
                    config::LoadConfig(loader_path, it->FileNameStem);
                }
            }
        }

        s_Instance = this;
    }

    Application::~Application()
    {
        Exit();
    }

    void Application::InitConsole()
    {
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    }

    void Application::render_UI(ImGuiIO* io)
    {
#ifdef _DEBUG
        ui::ui_main(io);
#else
        g_is_verified ? ui::ui_main(io) : ui::ui_init(io);
#endif
    }

    bool Application::Init()
    {
#ifdef _DEBUG
        InitConsole();
#endif

        g_is_running = true;

        m_window = std::make_shared<ImGuiWindow>("Toad", WINDOW_HEIGHT, WINDOW_WIDTH);
        m_window->SetUI(render_UI);
        m_window->StartWindow();

        //if (!SetupMenu()) return false;
        if (!pre_init())
            return false;

#ifndef _DEBUG
        //ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
        return true;
    }

    void Application::MainLoop()
    {
        while (m_window->IsActive())
        {
            //m_window->UpdateMenu();
            //MenuLoop();
			SLEEP(100);
        }
    }

    std::shared_mutex mutex;
    void Application::Exit() const
    {
        std::unique_lock lock(mutex);

        if (!g_is_running)
        {
            return;
        }

        std::cout << "closing\n";
        g_is_running = false;

        m_window->DestroyWindow();

        clean_up();
        stop_all_threads();

#ifdef _DEBUG
        FreeConsole();
#endif
    }

    Application* Application::Get()
    {
        return s_Instance;
    }
}
