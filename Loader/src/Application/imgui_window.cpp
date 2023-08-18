#include "toad.h"
#include "imgui_window.h"
#include <string>

#include "Fonts/fa-solid-900Font.h"
#include "imgui/backends/imgui_impl_dx9.h"
#include "imgui/backends/imgui_impl_win32.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace toad
{

ImGuiWindow::ImGuiWindow(std::string window_title, int win_height, int win_width)
	: m_windowName(std::move(window_title)), m_window_width(win_width), m_window_height(win_height) 
{
}

ImGuiWindow::~ImGuiWindow()
{
    DestroyWindow();
}

void ImGuiWindow::StartWindow()
{
    std::cout << "Creating window with name: " << m_windowName << std::endl;

    if (!m_isUIFuncSet)
    {
        std::cout << "The UI function has not been set for this window " << std::endl;
    }

    // there should be no duplicate window names 
    if (m_windowNameMap.contains(m_windowName))
    {
        std::cout << "Window " << m_windowName << " already exists! " << std::endl;
        assert(0);
        m_isRunning = false;
        return;
    }

    std::cout << "Starting window thread\n";
    m_windowThread = std::thread(&ImGuiWindow::CreateImGuiWindow, this, m_windowName, m_window_height, m_window_width);
}

void ImGuiWindow::DestroyWindow()
{
    std::lock_guard lock(m_destroyWindowMutex);

    if (!m_isRunning) return;

    std::cout << "Closing window: " << m_windowName << std::endl;

    m_windowNameMap.erase(m_windowName);
    m_windowHwndMap.erase(m_hwnd);

    if (m_windowThread.joinable())
        m_windowThread.join();

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(m_hwnd);
    ::UnregisterClassA(m_windowClassName.c_str(), m_wc.hInstance);

    m_isRunning = false;

}

ImGuiWindow::D3DProperties* ImGuiWindow::GetD3DProperties()
{
    return &m_d3dProperties;
}

HWND ImGuiWindow::GetHandle() const
{
    return m_hwnd;
}

bool ImGuiWindow::IsActive() const
{
    return m_isRunning && !m_shouldClose;
}

bool ImGuiWindow::IsFontUpdated() const
{
    return !m_update_font;
}

void ImGuiWindow::SetUI(const std::function<void(ImGuiIO* io)>& ui_func)
{
    m_isUIFuncSet = true;
    m_uiFunction = ui_func;
}

void ImGuiWindow::AddFontTTF(std::string_view pathTTF)
{
    m_update_font_path = pathTTF;
    m_update_font = true;
}

void ImGuiWindow::CreateImGuiWindow(const std::string& window_title, int win_height, int win_width)
{
    RECT desktop_rect = {};

    m_windowClassName = window_title + " class";

    GetWindowRect(GetDesktopWindow(), &desktop_rect);
    auto x = (desktop_rect.right - win_width) / 2;
    auto y = (desktop_rect.bottom - win_height) / 2;
    m_wc = { sizeof(m_wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, m_windowClassName.c_str(), NULL };
    ::RegisterClassExA(&m_wc);

    std::cout << "Creating window \n";
    m_hwnd = ::CreateWindowA(m_wc.lpszClassName, window_title.data(), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, x, y, win_width, win_height, NULL, NULL, m_wc.hInstance, nullptr);
    if (!m_hwnd)
    {
        std::cout << "Failed to create window " << GetLastError() << std::endl;

        UnregisterClassA(m_wc.lpszClassName, m_wc.hInstance);
        m_isRunning = false;
        return;
    }

    std::cout << "Initializing D3D\n";
    // Initialize Direct3D
    if (!CreateDeviceD3D())
    {
        std::cout << "Init of D3D failed\n";

        CleanupDeviceD3D();
    	UnregisterClassA(m_wc.lpszClassName, m_wc.hInstance);
        m_isRunning = false;

        return;
    }

    // Show the window
    ::ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(m_hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    m_io = &ImGui::GetIO(); (void)m_io;
    //m_io->ConfigWindowsMoveFromTitleBarOnly = true;
    m_io->Fonts->AddFontDefault();
    m_io->IniFilename = NULL; // disable imgui.ini file
    m_io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    m_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    static constexpr ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };

    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;

    m_io->Fonts->AddFontFromMemoryCompressedBase85TTF(base85_compressed_data_fa_solid_900, 24, &icons_config, icons_ranges);

    ImGui::StyleColorsDark();

    m_style = &ImGui::GetStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(m_hwnd);
    ImGui_ImplDX9_Init(m_d3dProperties.pD3DDevice);

    m_windowNameMap.insert({ window_title.data(), this });
    m_windowHwndMap.insert({ m_hwnd, this });
    std::cout << "Window " << window_title << " has successfully been created " << std::endl;

    while (m_isRunning && !m_shouldClose)
    {
        UpdateMenu();
    }
}

bool ImGuiWindow::CreateDeviceD3D()
{
    if ((m_d3dProperties.pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&m_d3dProperties.pD3DParams, sizeof(m_d3dProperties.pD3DParams));
    m_d3dProperties.pD3DParams.Windowed = TRUE;
    m_d3dProperties.pD3DParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_d3dProperties.pD3DParams.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    m_d3dProperties.pD3DParams.EnableAutoDepthStencil = TRUE;
    m_d3dProperties.pD3DParams.AutoDepthStencilFormat = D3DFMT_D16;
    m_d3dProperties.pD3DParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //m_d3dProperties.pD3DParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    auto hres = m_d3dProperties.pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_d3dProperties.pD3DParams, &m_d3dProperties.pD3DDevice);
    if (hres != D3D_OK)
    {
        std::cout << "Failed to create D3D Device Error: ";

	    if (hres == D3DERR_DEVICELOST)
	    {
            std::cout << "D3DERR_DEVICELOST\n";
	    }
        else if (hres == D3DERR_INVALIDCALL)
        {
            std::cout << "D3DERR_INVALIDCALL\n";
        }
        else if (hres == D3DERR_NOTAVAILABLE)
        {
            std::cout << "D3DERR_NOTAVAILABLE\n";
        }
        else if (hres == D3DERR_OUTOFVIDEOMEMORY)
        {
            std::cout << "D3DERR_OUTOFVIDEOMEMORY\n";
        }
        return false;
    }

    return true;
}

void ImGuiWindow::CleanupDeviceD3D()
{
    if (m_d3dProperties.pD3DDevice) { m_d3dProperties.pD3DDevice->Release(); m_d3dProperties.pD3DDevice = NULL; }
    if (m_d3dProperties.pD3D) { m_d3dProperties.pD3D->Release(); m_d3dProperties.pD3D = NULL; }
}

void ImGuiWindow::ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = m_d3dProperties.pD3DDevice->Reset(&m_d3dProperties.pD3DParams);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

void ImGuiWindow::UpdateMenu()
{
    MSG msg;
    while (::PeekMessageA(&msg, NULL, 0U, 0U, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessageA(&msg);
        if (msg.message == WM_QUIT)
        {
            m_shouldClose = true;
        }
    }

    if (m_shouldClose)
        return;

    if (m_update_font)
    {
        ImFont* res = nullptr;
        if (m_update_font_path == "Default")
        {
            res = m_io->Fonts->AddFontDefault();
        }
        else
        {
            res = m_io->Fonts->AddFontFromFileTTF(m_update_font_path.c_str(), 30.f);
        }
        assert(res != nullptr && "Adding font from path returned null");

        ImGui_ImplDX9_InvalidateDeviceObjects();
        m_update_font = false;
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    //ui
    m_uiFunction(m_io);

    // Rendering
    ImGui::EndFrame();

    m_d3dProperties.pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_d3dProperties.pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_d3dProperties.pD3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

    constexpr static auto clear_color = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    constexpr static D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
    m_d3dProperties.pD3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);

    if (m_d3dProperties.pD3DDevice->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        m_d3dProperties.pD3DDevice->EndScene();
    }

    if (m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    HRESULT result = m_d3dProperties.pD3DDevice->Present(nullptr, nullptr, nullptr, nullptr);

    // Handle loss of D3D9 device
    if (result == D3DERR_DEVICELOST && m_d3dProperties.pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        ResetDevice();
}

void ImGuiWindow::DefaultUIWindow(ImGuiIO* io)
{
    // match the window sizes 
    ImGui::SetNextWindowPos({ 0,0 });
    ImGui::SetNextWindowSize(io->DisplaySize);

    // transparent static imgui window 
    constexpr static auto window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground;

    ImGui::Begin("Default Window UI", nullptr, window_flags);
    center_text({ 1,0,0,1 }, "NO UI HAS BEEN SET FOR THIS WINDOW");
    ImGui::End();
}

ImGuiWindow* ImGuiWindow::GetWindowInstance(std::string_view window_name)
{
    auto it = m_windowNameMap.find(window_name.data());
    if (it != m_windowNameMap.end())
        return it->second;
    return nullptr;
}

ImGuiWindow* ImGuiWindow::GetWindowInstance(const HWND& hwnd)
{
    auto it = m_windowHwndMap.find(hwnd);
    if (it != m_windowHwndMap.end())
        return it->second;
    return nullptr;
}

LRESULT WINAPI ImGuiWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
	case WM_SIZE:
	    {
	        // There are other ways to get the current window instance, this is using a map
	        auto window = GetWindowInstance(hWnd);
	        if (!window)
	        {
	            std::cout << "couldn't get window instance of " << hWnd << std::endl;
	            return ::DefWindowProcA(hWnd, msg, wParam, lParam);
	        }

	        auto d3dproperties = window->GetD3DProperties();

	        if (d3dproperties->pD3DDevice != nullptr && wParam != SIZE_MINIMIZED)
	        {
	            d3dproperties->pD3DParams.BackBufferWidth = LOWORD(lParam);
	            d3dproperties->pD3DParams.BackBufferHeight = HIWORD(lParam);
	            window->ResetDevice();
	        }
	        return 0;
	    }

	case WM_SYSCOMMAND:
	    {
	        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
	            return 0;
	        break;
	    }

	case WM_DESTROY:
	    {
	        ::PostQuitMessage(0);
	        return 0;
	    }
    }
    return ::DefWindowProcA(hWnd, msg, wParam, lParam);
}

}
