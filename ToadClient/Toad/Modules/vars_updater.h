#pragma once
namespace toadll {

/**
 * @brief
 * Update important global variables in this thread
 */
class CVarsUpdater SET_MODULE_CLASS(CVarsUpdater)
{
public:
	void Update(const std::shared_ptr<c_Entity>& lPlayer) override;
};

}
