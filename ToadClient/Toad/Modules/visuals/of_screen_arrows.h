#pragma once

namespace toadll
{

///
/// IN TESTING
///
class COfScreenArrows SET_MODULE_CLASS(COfScreenArrows)
{
public:
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;
	void OnImGuiRender(ImDrawList* draw) override;
	void OnRender() override;

private:
	inline static std::vector<std::pair<Entity, Vec3>> m_screenPositions = {};
};

}
