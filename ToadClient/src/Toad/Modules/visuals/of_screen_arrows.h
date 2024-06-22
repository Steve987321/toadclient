#pragma once
#include <glm/vec3.hpp>

namespace toadll
{

///
/// IN TESTING
///
class COfScreenArrows SET_MODULE_CLASS(COfScreenArrows)
{
	using arrow = std::array<ImVec2, 3>;

public:
	void PreUpdate() override;
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;
	void OnImGuiRender(ImDrawList* draw) override;
private:
	std::vector<arrow> m_directions;
};

}
