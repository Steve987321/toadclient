#pragma once

namespace toad
{

class CNoClickDelay : public CModule
{
public:
	using CModule::CModule;

	void PreUpdate() override;
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;

public:
	static void Invoke(Minecraft* minecraft);
};

}

