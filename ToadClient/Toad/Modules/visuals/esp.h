#pragma once
#include <gl/GLU.h>

namespace toadll
{

class CEsp SET_MODULE_CLASS(CEsp)
{
private:
	inline static std::vector<BBox> m_bboxes;

	inline static std::atomic_bool m_canSave = true;

	inline static std::vector<Entity> m_playerList;
	std::vector<std::pair<std::string, BBox>> m_entityList;

private:
	/// Returns a vector of bounding boxes of the player list.
	/// 
	/// the bounding boxes are relative to the player 
	static std::vector<BBox> GetBBoxes(const std::shared_ptr<LocalPlayer>& lPlayer);

private:
	static void draw3dBox(const BBox& bbox);

	static void draw2dBox(const BBox& bbox);

public:
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;
	void OnRender() override;
};

}