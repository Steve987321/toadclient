#pragma once
namespace toadll
{

class CBridgeAssist SET_MODULE_CLASS(CBridgeAssist)
{
public:
	void PreUpdate() override;
	void Update(const std::shared_ptr<LocalPlayer>&lPlayer) override;

private:
	void Sneak();
	void UnSneak();

private:
	// parameters for the raytrace 
	Vec3 m_from = {  };
	const Vec3 m_direction = { 0, -1e6, 0 };

	// true if player is on the egde of a block
	// m_prev will store the previous value of m_isEdge, this will help when player has entered or left an edge
	bool m_is_edge = false, m_prev = false; 

	// used for block_esp::only_initiate_when_sneaking
	bool m_has_pressed_shift = false;
};

}
