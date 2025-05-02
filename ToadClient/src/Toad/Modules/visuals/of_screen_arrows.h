#pragma once
#include <glm/vec3.hpp>

namespace toad
{

///
/// IN TESTING
///
class COfScreenArrows : public CModule
{
public:
	using CModule::CModule;

	using arrow = std::array<ImVec2, 3>;

public:
	void PreUpdate() override;
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;
	void OnImGuiRender(ImDrawList* draw) override;
private:
	std::vector<arrow> m_directions;
};

}
