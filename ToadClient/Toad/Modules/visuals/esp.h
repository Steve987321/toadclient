#pragma once
#include <gl/GLU.h>

namespace toadll
{

class CEsp SET_MODULE_CLASS(CEsp)
{
public:
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;
	void OnRender() override;

private:
	std::shared_mutex m_bboxesMutex; 
	inline static std::vector<BBox> m_bboxes;

private:
	/// Returns a vector of bounding boxes of the player list.
	/// 
	/// the bounding boxes are relative to the player 
	std::vector<BBox> GetBBoxes() const;
};

}