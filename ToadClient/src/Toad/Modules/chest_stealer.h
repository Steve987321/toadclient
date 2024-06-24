#pragma once

namespace toadll
{

class CChestStealer SET_MODULE_CLASS(CChestStealer)
{
public:
	CChestStealer();
	void PreUpdate() override;
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;

private:
	POINT get_middle_of_screen() const;

private:
	// offset is from the middle of the window
	static inline std::array<POINT, 27> m_slotToMousePosOffset;

	// No randomization on these, because they are used for block hitting and not for clicking
	static inline void right_mouse_down(const POINT & pt);
	static inline void right_mouse_up(const POINT & pt);
};

}
