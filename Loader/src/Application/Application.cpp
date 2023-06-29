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
    bool CApplication::CreateDeviceD3D(const HWND& hWnd)
    {
        if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
            return false;

        // Create the D3DDevice
        ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
        g_d3dpp.Windowed = TRUE;
        g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
        g_d3dpp.EnableAutoDepthStencil = TRUE;
        g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
        g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
        //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
        if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
            return false;

        return true;
    }

    void CApplication::CleanupDeviceD3D()
    {
        if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
        if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
    }

    void CApplication::ResetDevice()
    {
        ImGui_ImplDX9_InvalidateDeviceObjects();
        HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
        if (hr == D3DERR_INVALIDCALL)
            IM_ASSERT(0);
        ImGui_ImplDX9_CreateDeviceObjects();
    }

    LRESULT WINAPI CApplication::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {

        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg)
        {
        case WM_SIZE:
            if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
            {
                g_d3dpp.BackBufferWidth = LOWORD(lParam);
                g_d3dpp.BackBufferHeight = HIWORD(lParam);
                ResetDevice();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        }
        return ::DefWindowProcW(hWnd, msg, wParam, lParam);
    }

    void CApplication::InitConsole()
    {
        AllocConsole();
        freopen_s(&f, "CONOUT$", "w", stdout);
    }

    bool CApplication::SetupMenu()
    {
        GetWindowRect(GetDesktopWindow(), &rect);
        auto x = float(rect.right - WINDOW_WIDTH) / 2.f;
        auto y = float(rect.bottom - WINDOW_HEIGHT) / 2.f;
        wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Toad Minecraft"), NULL };
        ::RegisterClassEx(&wc);

        hwnd = ::CreateWindow(wc.lpszClassName, _T("Toad"), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, x, y, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, wc.hInstance, NULL);

        // Initialize Direct3D
        if (!CreateDeviceD3D(hwnd))
        {
            CleanupDeviceD3D();
            ::UnregisterClass(wc.lpszClassName, wc.hInstance);
            return false;
        }

        // Show the window
        ::ShowWindow(hwnd, SW_SHOW);
        ::UpdateWindow(hwnd);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        io = &ImGui::GetIO(); (void)io;
        io->ConfigWindowsMoveFromTitleBarOnly = true;
        io->Fonts->AddFontDefault();
        static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
        ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
        io->Fonts->AddFontFromMemoryCompressedBase85TTF(base85_compressed_data_fa_solid_900, 24, &icons_config, icons_ranges);

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        style = &ImGui::GetStyle();
      /*  if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style->WindowRounding = 0.0f;
            style->Colors[ImGuiCol_WindowBg].w = 1.0f;
        }*/

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX9_Init(g_pd3dDevice);

        return true;
    }

    void CApplication::MenuLoop()
    {
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
            {
                toad::g_is_running = false;
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        //ui
        render_UI();

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

        constexpr auto clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    //void c_Application::UpdateCursorInfo()
    //{
    //    CURSORINFO ci{ sizeof(CURSORINFO) };
    //    if (GetCursorInfo(&ci))
    //    {
    //        auto handle = ci.hCursor;
    //        toad::is_cursor_visible = (int)handle < 1000000 || (int)handle == 13961697;

    //        //log_debug(ci.flags);
    //        //log_debug((int)handle);
    //        //log_debug(toad::clicker::cursor_visible);
    //        //toad::clicker::cursor_visible = int(handle) > 50000 & (int(handle) < 100000) || (int)handle == 65539;
    //    }
    //}

    bool CApplication::Init()
    {
#ifdef _DEBUG
        InitConsole();
#endif
        if (!SetupMenu()) return false;
        if (!pre_init()) return false;
#ifndef _DEBUG
        //ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
        g_is_running = true;
        return true;
    }

    void CApplication::MainLoop()
    {
        while (g_is_running)
        {
            MenuLoop();
			SLOW_SLEEP(1);
        }
    }

    std::shared_mutex mutex;
    void CApplication::Exit() const
    {
        std::unique_lock lock(mutex);
        std::cout << "closing\n";
        g_is_running = false;

        clean_up();
        stop_all_threads();

        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        CleanupDeviceD3D();
        ::DestroyWindow(hwnd);
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);

#ifdef _DEBUG
        fclose(f);
        FreeConsole();
#endif
    }
}