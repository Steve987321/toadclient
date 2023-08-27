#pragma once

namespace toadll
{

///
/// IN TESTING
///
class COfScreenArrows SET_MODULE_CLASS(COfScreenArrows)
{
public:
	void PreUpdate() override;
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;
	void OnImGuiRender(ImDrawList* draw) override;
	void OnRender() override;

};

}
