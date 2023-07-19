#pragma once
namespace toadll
{

///
/// Handles The Internal UI.
/// Shows when pressing switch to internal in the loader
///
class CInternalUI SET_MODULE_CLASS(CInternalUI)
{
public:
	/// used in CSwapBuffers::WndProcHook
	inline static int ShowMenuKey = VK_INSERT;

	inline static bool MenuIsOpen = true;

	/// when this is true the internal ui closes and opens the loader again.
	inline static bool ShouldClose = false;

public:
	void OnImGuiRender(ImDrawList * draw) override;
};

}

