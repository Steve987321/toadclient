#pragma once
#include <gl/GLU.h>

namespace toadll
{

class CEsp SET_MODULE_CLASS(CEsp)
{
public:
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;
	void OnRender() override;
	void PreUpdate() override;

	void OnImGuiRender(ImDrawList * draw) override;

private:
	Vec3 renderPos = {};
	Vec3 playerPos = {};

private:
	// for visuals 
	struct VisualEntity
	{
		BBox bb;
		Vec3 Pos;
		std::string name;
		int health;
		int hurttime; 
	};

private:
	inline static std::vector<VisualEntity> m_bboxes;

private:
	/// Returns a vector of bounding boxes of the player list.
	/// 
	/// the bounding boxes are relative to the player 
	std::vector<VisualEntity> GetBBoxes();

	std::vector<Vec3> GetBBoxVertices(const Vec3& min, const Vec3& max);
};

}