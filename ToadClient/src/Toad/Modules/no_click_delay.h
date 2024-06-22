#pragma once

namespace toadll
{

class CNoClickDelay SET_MODULE_CLASS(CNoClickDelay)
{
public:
	void PreUpdate() override;
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;

public:
	static void Invoke(const std::shared_ptr<Minecraft>& minecraft);
};

}

