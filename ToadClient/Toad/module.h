#pragma once

namespace toadll {

/**
 * @brief
 * interface for cheat features
 */
class CModule
{
public:
	virtual void Update(const std::shared_ptr<c_Entity>& lPlayer, float partialTick) = 0;
};

}

