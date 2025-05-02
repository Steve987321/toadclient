#pragma once

namespace toad
{
///
/// Handles The Internal UI.
/// Shows when pressing switch to internal in the loader
///
class CInternalUI : public CModule
{
public:
	using CModule::CModule;

	/// used in CSwapBuffers::WndProcHook
	inline static int& ShowMenuKey = Config::Get().ui_show_menu_key;

	inline static bool MenuIsOpen = true;

	/// when this is true the internal ui closes and opens the loader again.
	inline static bool ShouldClose = false;

public:
	void OnImGuiRender(ImDrawList * draw) override;

private:
	void ArrayList();
};

}

