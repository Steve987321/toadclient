#pragma once
namespace toadll
{

class CAimAssist SET_MODULE_CLASS(CAimAssist)
{
private:
	std::vector<EntityT> m_playerList;
public:
	void Update(const std::shared_ptr<LocalPlayerT>& lPlayer) override;
};

}