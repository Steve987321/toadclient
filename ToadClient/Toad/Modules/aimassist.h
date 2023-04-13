#pragma once
namespace toadll
{

class CAimAssist SET_MODULE_CLASS(CAimAssist)
{
private:
	float yawdiffSpeed = 0;

public:
	void Update(const std::shared_ptr<c_Entity>& lPlayer, float partialTick) override;
};

}