#pragma once

namespace toadll
{

class CEsp SET_MODULE_CLASS(CEsp)
{
private:
	std::unordered_map<const char*, vec2> playerListMap;
	std::vector<std::pair<const char*, vec2>> playerPositions;

public:
	void Update(const std::shared_ptr<c_Entity>& lPlayer, float partialTick) override;
public:
	_NODISCARD std::unordered_map<const char*, vec2> getMappedPlayerList() const;
};

}