#pragma once
namespace toadll
{

class CBlink SET_MODULE_CLASS(CBlink)
{
private:
	CTimer m_timer;
	inline static std::atomic_bool m_canSavePos = true;
	inline static std::vector<vec3> m_positions = {};

private:
	static void DisableBlink();

public:
	void Update(const std::shared_ptr<LocalPlayerT>& lPlayer) override;
	void OnRender() override;
};

}

