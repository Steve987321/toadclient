#pragma once
namespace toadll
{

class CAimAssist SET_MODULE_CLASS(CAimAssist)
{
public:
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;
};

}