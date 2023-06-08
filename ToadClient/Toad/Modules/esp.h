#pragma once

namespace toadll
{

class CEsp SET_MODULE_CLASS(CEsp)
{
private:
	struct EntityVisual
	{
		std::string name = "?";
		bbox b_box = {{0,0,0}, {0,0,0}};
		vec3 lasttickpos = {0,0,0};
		vec3 position = { 0,0,0 };
	};

private:
	std::vector<bbox> bboxxesdud = {};
	std::vector<float> modelview;
	std::vector<float> projection;

	std::vector<EntityVisual> m_entity_list;

public:
	void Update(const std::shared_ptr<c_Entity>& lPlayer) override;
	void OnRender() override;
};

}