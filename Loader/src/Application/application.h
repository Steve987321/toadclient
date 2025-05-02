#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

// TOAD
#include "Toad/imgui_window.h"
#include "Toad/config.h"
#include "Toad/Utils/ui_helpers.h"
#include "Toad/Utils/block_map.h"
#include "Toad/Utils/ui_helpers.h"
#include "Toad/Utils/ipc.h"
#include "Application/application.h"
#include "Application/ui.h"
#include "Toad/Modules/VisualizeClicker/visualize_clicker.h"

// use this when precision isn't required but the CPU should be saved
#define SLEEP(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

// #TODO: loader logger
#ifdef TOAD_LOADER
#define LOGDEBUG(msg, ...) std::cout << msg << std::endl;
#define LOGERROR(msg, ...) std::cout << msg << std::endl;
#define LOGWARN(msg, ...) std::cout << msg << std::endl;
#endif 

namespace toad
{
	const Window* get_injected_window();
	void set_injected_window(const Window& window);

	// only updated when still in init screen
	// contains the list of all minecraft windows that user can inject to  
	inline std::vector<Window> g_mc_window_list = {};

	inline Window g_injected_window{ {}, 0, {} };

	// will be true when injection was succesfull
	// will remain false if we haven't injected 
	inline bool g_is_verified = false;

	// ipc to dll 
	inline Utils::IPC g_ipc;

	namespace Application
	{
		inline std::filesystem::path dll_path;

		ImGuiWindow& GetWindow();
		bool PreInjectSetup();

		// scans for windows and updates window list
		void ScanWindows();
		void UpdateIPC(Utils::IPC& ipc);
		bool Init();
		void MainLoop();
		void Exit();
	};
}
