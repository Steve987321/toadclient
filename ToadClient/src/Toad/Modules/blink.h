#pragma once

namespace toad
{

class CBlink : public CModule
{
public:
	using CModule::CModule;

	void PreUpdate() override;
	void Update(const std::shared_ptr<LocalPlayer>&lPlayer) override;
	void OnRender() override;

private:
	Timer m_timer;

	std::atomic_bool m_can_save_position = true;

	/// extra flag that gets checked for when trying to enable blink
	bool m_can_enable = true;

	/// when having settings.bl_show_trail enabled,
	/// positions of the trail are stored here
	std::vector<Vec3> m_positions = {}; 

private:
	void DisableBlink();

};

}

