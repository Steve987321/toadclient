#pragma once

namespace toadll
{

class CChestStealer SET_MODULE_CLASS(CChestStealer)
{
public:
	CChestStealer();

public:
	void PreUpdate() override;
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;
	void OnImGuiRender(ImDrawList * draw) override;

	void UpdateSlotPosOffsets();

private:
	// sets up the path the mouse takes to grab items
	// Will also filter items depending on settings
	void SetupPath();

	POINT get_middle_of_screen() const;

private:
	// offset is from the middle of the window, these are for small chests
	static inline std::array<POINT, 27> m_slotToMousePosOffset{};

	// chest contents
	std::array<std::string, 27> m_chestContents{};

	// path for mouse, ends with -1
	std::deque<int> m_indexPath{};

	std::mutex m_mutex;

	bool m_isShiftDown = false;

	static inline void right_mouse_down(const POINT & pt);
	static inline void right_mouse_up(const POINT & pt);
};

}
