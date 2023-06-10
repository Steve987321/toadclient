#pragma once

namespace toadll
{

class CVarsUpdater SET_MODULE_CLASS(CVarsUpdater)
{
public:
	static float PartialTick;
	static float RenderPartialTick;

	static std::vector<std::shared_ptr<c_Entity>> PlayerList;

public:
	void Update(const std::shared_ptr<c_Entity>& lPlayer) override;
};

}