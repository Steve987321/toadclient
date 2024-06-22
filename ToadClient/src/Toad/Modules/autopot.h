#pragma once

namespace toadll
{

class CAutoPot SET_MODULE_CLASS(CAutoPot)
{
public:
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;
};

}