#pragma once
#include "visuals/draw_helpers.h"

namespace toadll
{

class CAimAssist SET_MODULE_CLASS(CAimAssist)
{
public:
	void PreUpdate() override;
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;

private:
	// finds a target and if found, sets the given target and target_position.
	void GetTarget(std::shared_ptr<c_Entity>& target, Vec3& target_position, const std::shared_ptr<LocalPlayer>&lPlayer);

	void ApplyAimRand(float yaw_diff, float pitch_diff, float speed);

	// sets success to false if not succeeded
	Vec3 GetAimPoint(const std::shared_ptr<LocalPlayer>& lPlayer, const Vec3& target_pos, bool& success);

};

}