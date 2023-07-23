#include "toad.h"
#include "window.h"
#include <string>

#include "Fonts/fa-solid-900Font.h"
#include "imgui/backends/imgui_impl_dx9.h"
#include "imgui/backends/imgui_impl_win32.h"

namespace toad
{

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Window::Window(std::string_view window_title, int win_height, int win_width)
{
    std::cout << "Creating window with name: " << window_title << std::endl;

    // there should be no duplicate window names 
    if (m_windowNameMap.contains(window_title.data()))
    {
        std::cout << "Window " << window_title << " already exists! "<< std::endl;
        m_isInvalid = true;
        return;
    }

    if (CreateImGuiWindow(window_title, win_height, win_width))
    {
        m_windowNameMap.insert({window_title.data(), this});
        std::cout << "Window " << window_title << " has successfully been created " << std::endl;
    }
}

Window::~Window()
{
    if (m_isInvalid) 
        return;

    m_windowNameMap.erase(m_windowName);
    m_windowHwndMap.erase(m_hwnd);

    DestroyWindow();
}

Window* Window::GetWindowInstance(std::string_view window_name)
{
    auto it = m_windowNameMap.find(window_name.data());
    if (it != m_windowNameMap.end())
        return it->second;
    return nullptr;
}

Window* Window::GetWindowInstance(const HWND& hwnd)
{
    auto it = m_windowHwndMap.find(hwnd);
    if (it != m_windowHwndMap.end())
        return it->second;
    return nullptr;
}

Window::D3DProperties& Window::GetD3DProperties()
{
    return m_d3dProperties;
}

void Window::DestroyWindow()
{
    if (m_isInvalid) return;

    std::cout << "closing window: " << m_windowName << std::endl;
    m_isInvalid = true;

    clean_up();
    stop_all_threads();

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(m_hwnd);
    ::UnregisterClass(m_wc.lpszClassName, m_wc.hInstance);
}

bool Window::CreateImGuiWindow(std::string_view window_title, int win_height, int win_width)
{
    RECT desktop_rect = {};
    std::wstringstream ss;
    ss << window_title.data() << L" class";
    
    GetWindowRect(GetDesktopWindow(), &desktop_rect);
    auto x = (desktop_rect.right - win_width) / 2;
    auto y = (desktop_rect.bottom - win_height) / 2;
    m_wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, ss.str().c_str(), NULL };
    ::RegisterClassEx(&m_wc);

    m_hwnd = ::CreateWindowA((LPCSTR)m_wc.lpszClassName, window_title.data(), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, x, y, win_width, win_height, NULL, NULL, m_wc.hInstance, this);

    m_windowHwndMap.insert({m_hwnd, this});

    // Initialize Direct3D
    if (!CreateDeviceD3D(m_hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(m_wc.lpszClassName, m_wc.hInstance);
        return false;
    }

    // Show the window
    ::ShowWindow(m_hwnd, SW_SHOW);
    ::UpdateWindow(m_hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    m_io = &ImGui::GetIO(); (void)m_io;
    m_io->ConfigWindowsMoveFromTitleBarOnly = true;
    m_io->Fonts->AddFontDefault();
    static constexpr ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };

    ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;

    m_io->Fonts->AddFontFromMemoryCompressedBase85TTF(base85_compressed_data_fa_solid_900, 24, &icons_config, icons_ranges);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    m_style = &ImGui::GetStyle();
    /*  if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
          style->WindowRounding = 0.0f;
          style->Colors[ImGuiCol_WindowBg].w = 1.0f;
      }*/

      // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(m_hwnd);
    ImGui_ImplDX9_Init(m_d3dProperties.pD3DDevice);

    return true;
}


bool Window::CreateDeviceD3D(const HWND& hWnd)
{
    if ((m_d3dProperties.pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(m_d3dProperties.pD3DParams, sizeof(*m_d3dProperties.pD3DParams));
    m_d3dProperties.pD3DParams->Windowed = TRUE;
    m_d3dProperties.pD3DParams->SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_d3dProperties.pD3DParams->BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    m_d3dProperties.pD3DParams->EnableAutoDepthStencil = TRUE;
    m_d3dProperties.pD3DParams->AutoDepthStencilFormat = D3DFMT_D16;
    m_d3dProperties.pD3DParams->PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //m_d3dProperties.D3Dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (m_d3dProperties.pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, m_d3dProperties.pD3DParams, &m_d3dProperties.pD3DDevice) < 0)
        return false;

    return true;
}

void Window::CleanupDeviceD3D()
{
    if (m_d3dProperties.pD3DDevice) { m_d3dProperties.pD3DDevice->Release(); m_d3dProperties.pD3DDevice = NULL; }
    if (m_d3dProperties.pD3D) { m_d3dProperties.pD3D->Release(); m_d3dProperties.pD3D = NULL; }
}

void Window::ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = m_d3dProperties.pD3DDevice->Reset(m_d3dProperties.pD3DParams);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

LRESULT WINAPI Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // There are other ways to get the correct window instance, this is using a map
    auto window = GetWindowInstance(hWnd);
    if (!window)
    {
        std::cout << "couldn't get window instance of " << hWnd << std::endl;
        return ::DefWindowProcW(hWnd, msg, wParam, lParam);
    }

    auto [pD3D, pD3DDevice, pD3DParams] = window->GetD3DProperties();

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (pD3DDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            pD3DParams->BackBufferWidth = LOWORD(lParam);
            pD3DParams->BackBufferHeight = HIWORD(lParam);
            window->ResetDevice();
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

}
