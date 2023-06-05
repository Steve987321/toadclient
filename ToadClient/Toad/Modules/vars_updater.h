#pragma once
namespace toadll {

class CVarsUpdater SET_MODULE_CLASS(CVarsUpdater)
{
public:
	void Update(const std::shared_ptr<c_Entity>& lPlayer) override;
};

}
