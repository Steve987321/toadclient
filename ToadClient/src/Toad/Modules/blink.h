#pragma once
namespace toadll
{

class CBlink SET_MODULE_CLASS(CBlink)
{
public:
	CBlink();

public:
	void PreUpdate() override;
	void Update(const std::shared_ptr<LocalPlayer>&lPlayer) override;
	void OnRender() override;

private:
	Timer m_timer;

	std::atomic_bool m_can_save_position = true;

	/// extra flag that gets checked for when trying to enable blink
	bool m_can_enable = true;

	/// when having blink::show_trail enabled,
	/// positions of the trail are stored here
	std::vector<Vec3> m_positions = {}; 

private:
	void DisableBlink();

};

}

