#pragma once
namespace toadll
{

class CAimAssist SET_MODULE_CLASS(CAimAssist)
{
private:
	std::vector<Entity> m_playerList;
public:
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;
};

}