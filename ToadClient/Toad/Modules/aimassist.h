#pragma once
#include "visuals/draw_helpers.h"

namespace toadll
{

class CAimAssist SET_MODULE_CLASS(CAimAssist)
{
public:
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;
	void PreUpdate() override;

private:
	Vec3 getClosesetPoint(const BBox& bb, const Vec3& from) const
	{
		Vec3 closestPoint;

		// calculate the closest point on each axis
		closestPoint.x = std::clamp(from.x, bb.min.x, bb.max.x);
		closestPoint.y = std::clamp(from.y, bb.min.y, bb.max.y);
		closestPoint.z = std::clamp(from.z, bb.min.z, bb.max.z);

		return closestPoint;
	}
};

}