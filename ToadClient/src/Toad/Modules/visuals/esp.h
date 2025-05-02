#pragma once
#include <gl/GLU.h>

namespace toad
{

class CEsp : public CModule
{
public:
	using CModule::CModule;

	// for visuals 
	struct VisualEntity
	{
		VisualEntity(const BBox& bb, const Vec3& pos, const char* name, int health, bool sneaking = false)
			: bb(bb), pos(pos), health(health), sneaking(sneaking)
		{
			strncpy_s(this->name, name, 32);
		}

		BBox bb;
		Vec3 pos;
		char name[32];
		int health;
		bool sneaking;
	};

public:
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;
	void OnRender() override;
	void PreUpdate() override;

	void DrawPlayerInfo(ImDrawList * draw, const VisualEntity& ve, const Vec3& lPlayerPos);
	void OnImGuiRender(ImDrawList * draw) override;

private:
	Vec3 renderPos = {};
	Vec3 playerPos = {};

private:
	inline static std::mutex m_boxMutex;
	inline static std::vector<VisualEntity> m_bboxes;
private:
	/// Returns a vector of bounding boxes of the player list.
	/// 
	/// the bounding boxes are relative to the player 
	std::vector<VisualEntity> GetBBoxes();

	std::vector<Vec3> GetBBoxVertices(const Vec3& min, const Vec3& max);
};

}