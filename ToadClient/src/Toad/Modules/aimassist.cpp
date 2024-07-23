#include "pch.h"
#include "Toad/toadll.h"
#include "aimassist.h"

using namespace toad;
using namespace toadll::math;

namespace toadll 
{

AimBoost::AimBoost(float speed_mult_min, float speed_mult_max, const Vec2& frequency_range, bool continuous)
	: speed_mult_min(speed_mult_min), speed_mult_max(speed_mult_max),
	frequency_min_ms((uint32_t)frequency_range.x), frequency_max_ms((uint32_t)frequency_range.y),
	continuous(continuous)
{
	speed_mult = (speed_mult_min + speed_mult_max) / 2;
	prev_speed_mult = speed_mult;
}


float AimBoost::Delay()
{
	if (timer.Elapsed<>() >= frequency_ms)
	{
		if (continuous)
		{
			prev_speed_mult = rand_float(speed_mult_min, speed_mult_max);
		}
		else
		{
			prev_speed_mult = speed_mult;
			speed_mult = rand_float(speed_mult_min, speed_mult_max);
		}

		frequency_ms = rand_int(frequency_min_ms, frequency_max_ms);
		timer.Start();
	}

	return slerp(prev_speed_mult, continuous ? speed_mult : 1.f, timer.Elapsed<>() / frequency_ms);
}

CAimAssist::CAimAssist()
{
	Enabled = &aa::enabled;
}

void CAimAssist::PreUpdate()
{
	WaitIsEnabled();
	WaitIsVerified();
	SLEEP(5);
}

void CAimAssist::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	// destination aiming point 
	static Vec3 aim_point;

	// our (locked) target
	static std::shared_ptr<c_Entity> target = nullptr;
	static Vec3 target_pos = { 0, 0, 0 };

	// horizontal and vertical speed
	float speed = aa::speed * 3;

	// whether we should look for a target
	bool get_target = true;

	//std::cout << "AA, enabled, cursor shown, alwasy aim , mdown :" << aa::enabled << " " << is_cursor_shown << " " << aa::always_aim << " " << static_cast<bool>(GetAsyncKeyState(VK_LBUTTON)) << std::endl;
	if (!*Enabled || CVarsUpdater::IsInGui)
	{
		SLEEP(250);
		return;
	}

	if (!aa::always_aim && !GetAsyncKeyState(VK_LBUTTON))
	{
		SLEEP(100);
		return;
	}

	if (aa::break_blocks && GetAsyncKeyState(VK_LBUTTON))
	{
		if (MC->getMouseOverTypeStr() == "BLOCK")
		{
			SLEEP(50);
			return;
		}
	}

	// check if target is still a valid target (in fov and in distance)
	if (aa::lock_aim && target != nullptr)
	{
		target_pos = target->getPosition();

		// check if the target is still inside fov and distance bounds
		if (abs(wrap_to_180(-(lPlayer->Yaw - get_angles(lPlayer->Pos, target_pos).first))) <= aa::fov
			&&
			target_pos.dist(lPlayer->Pos) <= aa::distance)
		{
			get_target = false;
		}
	}
	
	if (get_target)
	{
		GetTarget(target, target_pos, lPlayer);
	}

	if (target == nullptr)
	{
		SLEEP(10);
		return;
	}

	if (g_curr_client == MC_CLIENT::Lunar_171)
		target_pos.y += 1.6f;

	bool success = true;
	aim_point = GetAimPoint(lPlayer, target_pos, success);
	if (!success)
	{
		SLEEP(10);
		return;
	}
 
	const auto [yawtarget, pitchtarget] = get_angles(lPlayer->Pos, aim_point);

	float yaw_diff = wrap_to_180(-(lPlayer->Yaw - yawtarget));
	float pitch_diff = wrap_to_180(-(lPlayer->Pitch - pitchtarget));

	ApplyAimRand(lPlayer, yaw_diff, pitch_diff, speed);
}

void CAimAssist::GetTarget(std::shared_ptr<c_Entity>& target, Vec3& target_pos, const std::shared_ptr<LocalPlayer>& lPlayer)
{
	// left: dist, right: enitity, position
	std::vector<std::pair<float, std::pair<std::shared_ptr<c_Entity>, Vec3>>> distances = {};

	float lowestHealth = FLT_MAX;
	// for getting closest to crosshair
	float minimal_angle_diff = aa::fov / 2.f;

	target = nullptr;

	// get a target
	for (const auto& e : MC->getPlayerList())
	{
		auto entityPos = e->getPosition();
		auto entityHealth = e->getHealth();

		auto distance = MC->getLocalPlayer()->getPosition().dist(entityPos);
		if (distance > aa::distance || distance < 0.2f) continue;
		if (e->isInvisible() && !aa::invisibles) continue;

		float yaw_diff = abs(wrap_to_180(-(lPlayer->Yaw - get_angles(lPlayer->Pos, entityPos).first)));
		if (yaw_diff > aa::fov / 2)
			continue;

		distances.emplace_back(distance, std::make_pair(e, entityPos));
		if (aa::target_mode == AA_TARGET::FOV)
		{
			if (yaw_diff < minimal_angle_diff)
			{
				minimal_angle_diff = yaw_diff;
				target_pos = entityPos;
				target = e;
			}
		}
		else if (aa::target_mode == AA_TARGET::HEALTH)
		{
			if (entityHealth < lowestHealth)
			{
				lowestHealth = entityHealth;
				target_pos = entityPos;
				target = e;
			}
		}
	}

	// atleast one other player
	if (distances.empty())
	{
		SLEEP(10);
		return;
	}

	// getting target by distance
	if (aa::target_mode == AA_TARGET::DISTANCE)
	{
		auto t = std::ranges::min_element(distances, [&](const auto& l, const auto& r)
			{
				const float l_yaw_diff = abs(wrap_to_180(-(lPlayer->Yaw - get_angles(lPlayer->Pos, l.second.second).first)));
				return l.first < r.first && l_yaw_diff < minimal_angle_diff;
			});

		target = t->second.first;
		target_pos = t->second.first->getPosition();
	}
	else if (aa::target_mode == AA_TARGET::HEALTH)
	{
		const float l_yaw_diff = abs(wrap_to_180(-(lPlayer->Yaw - get_angles(lPlayer->Pos, target_pos).first)));
		if (l_yaw_diff > minimal_angle_diff) // target out of fov range
		{
			SLEEP(10);
			return;
		}
	}
}

void CAimAssist::ApplyAimRand(const std::shared_ptr<LocalPlayer>& lPlayer, float yaw_diff, float pitch_diff, float speed)
{
	float abs_yaw_diff = abs(yaw_diff);
	float abs_pitch_diff = abs(pitch_diff);
	if (abs_yaw_diff < 1.f && abs_pitch_diff < 1.f)
	{
		return;
	}

	yaw_diff += rand_float(-0.2f, 0.2f);
	pitch_diff += rand_float(-0.2f, 0.2f);

	const int rand_100 = rand_int(0, 100);

	if (static bool once = false; !once || reaction_time_timer.Elapsed<>() > aa::reaction_time)
	{
		float pitch_boosts = pitch_boost.Delay() * pitch_boost2.Delay();
		float yaw_boosts = yaw_boost.Delay() * yaw_boost2.Delay();

		if (!aa::horizontal_only)
		{
			pitchdiff_speed = pitch_diff / (1000.f / (speed * pitch_boosts));

			if (abs(pitchdiff_speed) < 0.03f)
				pitchdiff_speed = pitchdiff_speed < 0 ? -0.03f : 0.03f;
		}
		yawdiff_speed = yaw_diff / (1000.f / (speed * yaw_boosts));

		if (abs(yawdiff_speed) < 0.03f)
			yawdiff_speed = yawdiff_speed < 0 ? -0.03f : 0.03f;

		once = true;
		reaction_time_timer.Start();
	}

	auto editable_local_player = MC->getLocalPlayer();
	if (!editable_local_player)
	{
		SLEEP(10);
		return;
	}

	static Timer pitch_rand_timer;
	static int pitchrand = rand_int(-1, 1);

	const int pitch_update_ms_min_low = 100;
	const int pitch_update_ms_max_low = 300;
	const int pitch_update_ms_min_high = 400;
	const int pitch_update_ms_max_high = 600;
	static int pitch_update_ms_min = 100;
	static int pitch_update_ms_max = 600;

	static int pitchupdatems = rand_int(pitch_update_ms_min, pitch_update_ms_max);
	static float pitchrandsmooth = 0;
	static int pitchrandbegin = 0;

	if (abs_yaw_diff > abs_pitch_diff && abs_yaw_diff > 5.f)
	{
		if ((int)pitch_rand_timer.Elapsed<>() > pitchupdatems)
		{
			pitchrandbegin = pitchrand;

			float t = (std::clamp(abs_yaw_diff, 10.f, 50.f) - 9.99f) / 40.f;
			pitch_update_ms_min = (int)std::lerp(pitch_update_ms_min_low, pitch_update_ms_min_high, t);
			pitch_update_ms_max = (int)std::lerp(pitch_update_ms_max_low, pitch_update_ms_max_high, t);
			int travel_distance = 2;
			pitchupdatems = rand_int(pitch_update_ms_min, pitch_update_ms_max);

			if (abs_yaw_diff > 30.f)
				travel_distance = 2;
			else
				travel_distance = 1;

			if (pitch_diff > 0)
				pitchrand = rand_int(1, travel_distance);
			else
				pitchrand = rand_int(-travel_distance, -1);

			pitch_rand_timer.Start();
		}

		pitchrandsmooth = slerp((float)pitchrandbegin, (float)pitchrand, pitch_rand_timer.Elapsed<>() / pitchupdatems);
	}
	else
	{
		pitchrandsmooth = std::lerp((float)pitchrandsmooth, 0.f, std::clamp(CVarsUpdater::PartialTick * 0.1f, 0.f, 1.f));
	}

	//static POINT prev_pt{ 0, 0 };
	//POINT pt{};

	//if (static bool once = true; once)
	//{
	//	GetCursorPos(&prev_pt);
	//	once = false;
	//}

	//GetCursorPos(&pt);

	//POINT pt_delta{ prev_pt.x - pt.x, prev_pt.y - pt.y };

	//GetCursorPos(&prev_pt);
	//std::cout << pt_delta.x << ' ' << pt_delta.y << std::endl;

	//HGetRawInputData::x = (int)yawdiff_speed + (int)pt_delta.x;
	//HGetRawInputData::y = (int)(pitchdiff_speed + pitchrandsmooth) + (int)pt_delta.y;
	//HGetRawInputData::move_mouse = true;

	POINT pt; 
	GetCursorPos(&pt);
	SetCursorPos(pt.x + (int)yawdiff_speed, pt.y + (int)(pitchdiff_speed + pitchrandsmooth));
}

Vec3 CAimAssist::GetAimPoint(const std::shared_ptr<LocalPlayer>& lPlayer, const Vec3& target_pos, bool& success)
{
	if (aa::aim_at_closest_point) // aims at the closest point of target
	{
		BBox playerbb =
			g_curr_client == MC_CLIENT::Lunar_189
			? BBox({ target_pos.x - 0.3f, target_pos.y - 1.6f, target_pos.z - 0.3f }, { target_pos.x + 0.3f, target_pos.y + 0.2f, target_pos.z + 0.3f })
			: BBox({ target_pos.x - 0.3f, target_pos.y - 1.6f, target_pos.z - 0.3f }, { target_pos.x + 0.3f, target_pos.y + 0.2f, target_pos.z + 0.3f });
		Vec3 closest_corner = get_closest_point(playerbb, lPlayer->Pos);
		return closest_corner;
	}
	else // aims to target if players aim is not inside hitbox 
	{
		const std::array<Vec3, 4> bbox_corners
		{
			Vec3{ target_pos.x - 0.3f, target_pos.y - 1.6f, target_pos.z + 0.3f },
			Vec3{ target_pos.x - 0.3f, target_pos.y - 1.6f, target_pos.z - 0.3f },
			Vec3{ target_pos.x + 0.3f, target_pos.y + 0.2f, target_pos.z - 0.3f },
			Vec3{ target_pos.x + 0.3f, target_pos.y + 0.2f, target_pos.z + 0.3f },
		};

		const std::array<float, 4> yawdiffs = {
			wrap_to_180(-(lPlayer->Yaw - get_angles(lPlayer->Pos, bbox_corners.at(0)).first)),
			wrap_to_180(-(lPlayer->Yaw - get_angles(lPlayer->Pos, bbox_corners.at(1)).first)),
			wrap_to_180(-(lPlayer->Yaw - get_angles(lPlayer->Pos, bbox_corners.at(2)).first)),
			wrap_to_180(-(lPlayer->Yaw - get_angles(lPlayer->Pos, bbox_corners.at(3)).first)),
		};

		float yawdiff_to_pos = wrap_to_180(-(lPlayer->Yaw - get_angles(lPlayer->Pos, target_pos).first));

		if (yawdiff_to_pos < 0)
		{
			if (*std::ranges::max_element(yawdiffs) > 0)
			{
				success = false;
				return {};
			}
		}
		else
		{
			if (*std::ranges::min_element(yawdiffs) < 0)
			{
				success = false;
				return {};
			}
		}

		return target_pos;
	}
}

}
