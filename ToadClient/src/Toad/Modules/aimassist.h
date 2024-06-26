#pragma once
#include "visuals/draw_helpers.h"

namespace toadll
{

class AimBoost
{
public:
	AimBoost(float speed_mult_min, float speed_mult_max, const Vec2& frequency_range, bool continuous);
	AimBoost() = default;

public:
	float speed_mult_min = 0.5f;
	float speed_mult_max = 1.8f;
	uint32_t frequency_min_ms = 400;
	uint32_t frequency_max_ms = 700;
	uint32_t frequency_ms = 500;

	// when true interpolate to new value instead of to 1
	bool continuous = true;

public:
	float Delay();

private:
	float prev_speed_mult = 1.f;
	float speed_mult = 1.f;
	Timer timer;
};

class CAimAssist SET_MODULE_CLASS(CAimAssist)
{
public:
	CAimAssist();

public:
	void PreUpdate() override;
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;

private:
	// finds a target and if found, sets the given target and target_position.
	void GetTarget(std::shared_ptr<c_Entity>& target, Vec3& target_position, const std::shared_ptr<LocalPlayer>&lPlayer);

	// randomizes yaw and pitch then applies it to the player 
	void ApplyAimRand(const std::shared_ptr<LocalPlayer>&lPlayer, float yaw_diff, float pitch_diff, float speed);

	// sets success to false if not succeeded
	Vec3 GetAimPoint(const std::shared_ptr<LocalPlayer>& lPlayer, const Vec3& target_pos, bool& success);

	enum class AASTYLE
	{
		PREDICT,
		SMOOTH,
	} style = AASTYLE::PREDICT;
};

}