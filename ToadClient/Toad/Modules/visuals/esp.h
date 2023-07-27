#pragma once
#include <gl/GLU.h>

namespace toadll
{

class CEsp SET_MODULE_CLASS(CEsp)
{
public:
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;
	void OnRender() override;

	// tmp 
	void OnImGuiRender(ImDrawList* draw) override
{
		std::stringstream ss;
		ss << debuggingvector3;
		draw->AddText({ 0,0 }, IM_COL32_WHITE, ss.str().c_str());
}
	Vec3 debuggingvector3 = {};

private:
	std::mutex m_bboxesMutex; 
	inline static std::vector<BBox> m_bboxes;

private:
	/// Returns a vector of bounding boxes of the player list.
	/// 
	/// the bounding boxes are relative to the player 
	std::vector<BBox> GetBBoxes();
};

}