#pragma once
namespace toadll
{

class CBridgeAssist SET_MODULE_CLASS(CBridgeAssist)
{
public:
	void Update(const std::shared_ptr<LocalPlayer>&lPlayer) override;
private:
	Timer m_pressedJumpTimer;

	Vec3 m_from = {  };
	Vec3 m_direction = { 0, -1e6, 0 };
	bool m_isEdge = false;
};

}
