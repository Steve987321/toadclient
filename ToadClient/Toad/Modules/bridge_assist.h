#pragma once
namespace toadll
{

class CBridgeAssist SET_MODULE_CLASS(CBridgeAssist)
{
public:
	void Update(const std::shared_ptr<LocalPlayer>&lPlayer) override;

private:
	void Sneak()
	{
		m_isEdge = true;
		if (!m_prev)
		{
			send_key(VK_SHIFT, true);
			m_prev = true;
		}
	}

	void UnSneak()
	{
		m_isEdge = false;
		if (m_prev)
		{
			send_key(VK_SHIFT, false);
			m_prev = false;
		}
	}

private:
	// parameters for the raytrace 
	Vec3 m_from = {  };
	const Vec3 m_direction = { 0, -1e6, 0 };

	// true if player is on the egde of a block
	// m_prev will store the previous value of m_isEdge, this will help when player has entered or left an edge
	bool m_isEdge = false, m_prev = false; 
};

}
