#pragma once
namespace toadll
{

class CAimAssist SET_MODULE_CLASS(CAimAssist)
{
private:
	float yawdiffSpeed = 0;
	float reaction_time_timer = 0;

public:
	//CAimAssist() : CModule() {}

public:
	void Update(const std::shared_ptr<c_Entity>& lPlayer) override;
};

}