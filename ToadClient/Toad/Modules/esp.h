#pragma once

namespace toadll::esp
{
	//inline std::vector<std::pair<vec2, const char*>> renderNames;
	inline std::unordered_map<const char*, vec2> playerListMap;

	void UpdatePlayerMap(const std::shared_ptr<c_Entity>& lPlayer);

}
