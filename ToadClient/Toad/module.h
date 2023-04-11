#pragma once

namespace toadll {

/**
 * @brief
 * interface for cheat features
 */
class CModule
{
protected:
	bool is_enabled = false;
	int toggle_key = -1;
public:
	virtual void Update(const std::shared_ptr<c_Entity>& lPlayer, float partialTick) = 0;

	virtual void SetEnabled(bool val) { is_enabled = val; }
};

}

