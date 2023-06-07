#pragma once

namespace toadll
{
	struct EntityVisual
	{
		const char* name;
		vec4 bounding_box2d;
	};

	inline std::vector<std::shared_ptr<EntityVisual>> entityVisualList = {};

class CEsp SET_MODULE_CLASS(CEsp)
{

public:
	void Update(const std::shared_ptr<c_Entity>& lPlayer) override;
	void OnRender(ImDrawList* draw) override;
};

}