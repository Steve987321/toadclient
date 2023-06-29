#pragma once
namespace toadll
{
	class CInternalUI SET_MODULE_CLASS(CInternalUI)
	{
	public:
		inline static int ShowMenuKey = VK_INSERT;
		inline static bool MenuIsOpen = true;
		inline static bool ShouldClose = false;

	public:
		void OnImGuiRender(ImDrawList * draw) override;
	};
}

