#include "pch.h"
#include "Toad/Toad.h"
#include "aimassist.h"

using namespace toad;

namespace toadll {

void CAimAssist::Update(const std::shared_ptr<LocalPlayer>& lPlayer)
{
	//std::cout << "AA, enabled, cursor shown, alwasy aim , mdown :" << aa::enabled << " " << is_cursor_shown << " " << aa::always_aim << " " << static_cast<bool>(GetAsyncKeyState(VK_LBUTTON)) << std::endl;
	if (!aa::enabled || CVarsUpdater::IsInGui)
	{
		SLEEP(250);
		return;
	}
	if (!aa::always_aim && !GetAsyncKeyState(VK_LBUTTON))
	{
		SLEEP(100);
		return;
	}

	std::vector <std::pair<float, Entity>> distances = {};

	static std::shared_ptr<Entity> target = nullptr;
	float speed = aa::speed;
	float minimal_angle_diff = aa::fov / 2.f;

	bool skip_get_target = false;

	if (aa::lock_aim && target != nullptr)
	{
		if (
			abs(wrap_to_180(-(lPlayer->Yaw - get_angles(lPlayer->Pos, target->Pos).first))) <= aa::fov
			&&
			target->Pos.dist(lPlayer->Pos) <= aa::distance
			)
			
			skip_get_target = true;
	}
	
	if (!skip_get_target)
	{
		target = nullptr;

		m_playerList = CVarsUpdater::PlayerList;

		//get a target
		for (const auto& player : m_playerList)
		{
			if (lPlayer->Pos.dist(player.Pos) > aa::distance) continue;
			if (player.Invis && !aa::invisibles) continue;

			distances.emplace_back(lPlayer->Pos.dist(player.Pos), player);
			if (aa::targetFOV)
			{
				float yaw_diff = abs(wrap_to_180(-(lPlayer->Yaw - get_angles(lPlayer->Pos, player.Pos).first)));
				if (yaw_diff < minimal_angle_diff)
				{
					minimal_angle_diff = yaw_diff;
					target = std::make_shared<Entity>(player);
				}
			}
		}
		// atleast one other player
		if (distances.empty())
		{
			SLEEP(1);
			return;
		} 

		// getting target by distance
		if (!aa::targetFOV)
		{
			auto t = std::ranges::min_element(distances, [&](const auto& l, const auto& r)
			{
				const float l_yaw_diff = abs(wrap_to_180(-(lPlayer->Yaw - get_angles(lPlayer->Pos, l.second.Pos).first)));
				return l.first < r.first && l_yaw_diff < minimal_angle_diff;
			});

			target = std::make_shared<Entity>(t->second);
		}
	}

	if (target == nullptr)
	{
		SLEEP(1);
		return;
	}

	auto targetPos = target->Pos;
	auto lplayer_pos = lPlayer->Pos;
	Vec3 aimPoint;

	// hitbox vertices
	const std::vector<Vec3> bbox_corners
	{
		{ targetPos.x - 0.3f, targetPos.y, targetPos.z + 0.3f },
		{ targetPos.x - 0.3f, targetPos.y, targetPos.z - 0.3f },
		{ targetPos.x + 0.3f, targetPos.y, targetPos.z - 0.3f },
		{ targetPos.x + 0.3f, targetPos.y, targetPos.z + 0.3f },
	};

	if (aa::aim_at_closest_point) // aims at the closest corner of target
	{
		const std::vector<float> corner_distances = {
			bbox_corners[0].dist(lplayer_pos),
			bbox_corners[1].dist(lplayer_pos),
			bbox_corners[2].dist(lplayer_pos),
			bbox_corners[3].dist(lplayer_pos),
		};

		auto closest_corner = bbox_corners[std::distance(corner_distances.begin(), std::ranges::min_element(corner_distances))];

		aimPoint = closest_corner;
	}
	else // aims to target if players aim is not inside hitbox 
	{
		auto lplayer_yaw = lPlayer->Yaw;
		auto yawdiff_to_pos = wrap_to_180(-(lplayer_yaw - get_angles(lplayer_pos, targetPos).first));

		const std::vector<float> yawdiffs = {
			wrap_to_180(-(lplayer_yaw - get_angles(lplayer_pos, bbox_corners.at(0)).first)),
			wrap_to_180(-(lplayer_yaw - get_angles(lplayer_pos, bbox_corners.at(1)).first)),
			wrap_to_180(-(lplayer_yaw - get_angles(lplayer_pos, bbox_corners.at(2)).first)),
			wrap_to_180(-(lplayer_yaw - get_angles(lplayer_pos, bbox_corners.at(3)).first)),
		};

		if (!aa::aim_at_target)
		{
			if (yawdiff_to_pos < 0)
			{
				if (*std::ranges::max_element(yawdiffs) > 0)
				{
					SLEEP(1);
					return;
				}
			}
			else
			{
				if (*std::ranges::min_element(yawdiffs) < 0)
				{
					SLEEP(1);
					return;
				}
			}
		}

		aimPoint = target->Pos;
	}

	auto [yawtarget, pitchtarget] = get_angles(lPlayer->Pos, aimPoint/*target->get_position()*/);

	auto lyaw = lPlayer->Yaw;
	auto lpitch = lPlayer->Pitch;

	float yawDiff = wrap_to_180(-(lyaw - yawtarget));
	float absYawDiff = abs(yawDiff);
	if (!aa::aim_at_target && absYawDiff < 3.f)
	{
		SLEEP(1);
		return;
	}
	float pitchDiff = wrap_to_180(-(lpitch - pitchtarget));

	if (!aa::targetFOV) // don't have to check if this is enabled because already checked
		if (absYawDiff > minimal_angle_diff)
		{
			SLEEP(1);
			return;
		}
	// got target and yaw and pitch offsets

	yawDiff += toadll::RandFloat(-2.f, 2.f);
	pitchDiff += toadll::RandFloat(-2.f, 2.f);

	const int rand_100 = toadll::RandInt(0, 100);

	static Timer speed_rand_timer;
	static Timer reaction_time_timer;
	static float long_speed_modifier = 1;
	static float prev_long_speed_modifier = 1;
	static float long_speed_modifier_smooth = 1;

	if (speed_rand_timer.Elapsed<>() >= 400)
	{
		prev_long_speed_modifier = long_speed_modifier;
		long_speed_modifier = toadll::RandFloat(0.7f, 1.3f);
		speed_rand_timer.Start();
		//std::cout << "reset :" << long_speed_modifier << std::endl;
	}

	long_speed_modifier_smooth = std::lerp(prev_long_speed_modifier, long_speed_modifier, speed_rand_timer.Elapsed<>() / 400);
	static float yawdiff_speed = 0;

	if (static bool once = false; !once || reaction_time_timer.Elapsed<>() > aa::reaction_time)
	{
		yawdiff_speed = yawDiff / (15000.f / speed * long_speed_modifier_smooth);
		once = true;
		reaction_time_timer.Start();
	}

	auto editable_local_player = MC->getLocalPlayer();
	if (!editable_local_player)
	{
		SLEEP(1);
		return;
	}

	auto updated_yaw = editable_local_player->getRotationYaw();
	//log_Debug("{} | %f = %f + %f", target->get_name().c_str(), lyaw + yawdiffSpeed, lyaw, yawdiffSpeed);
	editable_local_player->setRotationYaw(updated_yaw + yawdiff_speed);
	editable_local_player->setPrevRotationYaw(updated_yaw + yawdiff_speed);

	// pitch randomization
	static Timer pitch_rand_timer;
	static Timer pitch_ms_rand_timer;
	static float pitchrand = RandFloat(-0.0100f, 0.0100f);

	static const Vec2 pitch_update_ms = {500, 800};
	static float pitch_update_ms_min = pitch_update_ms.x;
	static float pitch_update_ms_max = pitch_update_ms.y;
	static float pitch_update_ms_min_smooth = pitch_update_ms.x;
	static float pitch_update_ms_max_smooth = pitch_update_ms.y;

	static float pitchupdatems = RandFloat(pitch_update_ms_min_smooth, pitch_update_ms_max_smooth);
	static float pitchrandsmooth = 0;
	static float pitchrandbegin = 0;
	auto updated_pitch = editable_local_player->getRotationPitch();
	if (pitch_rand_timer.Elapsed<>() > pitchupdatems)
	{
		pitchrandbegin = pitchrand;
		pitchupdatems = RandFloat(pitch_update_ms_min_smooth, pitch_update_ms_max_smooth);
		pitchrand = RandFloat(-0.0080f, 0.0080f);
		pitch_rand_timer.Start();
	}

	pitchrandsmooth = slerp(pitchrandbegin, pitchrand, pitch_rand_timer.Elapsed<>() / pitchupdatems);
	pitch_update_ms_min_smooth = std::lerp(pitch_update_ms_min, pitch_update_ms.x, std::clamp(pitch_ms_rand_timer.Elapsed<>() / 200.f, 0.f, 1.f));
	pitch_update_ms_max_smooth = std::lerp(pitch_update_ms_max, pitch_update_ms.y, std::clamp(pitch_ms_rand_timer.Elapsed<>() / 200.f, 0.f, 1.f));
	if (rand_100 < 5)
	{
		const auto rand_f = RandFloat(250.f, 400.f);
		pitch_update_ms_min = std::clamp(rand_f - 10, 250.f, 400.f);
		pitch_update_ms_max = std::clamp(rand_f + 10, 250.f, 400.f);
		pitch_ms_rand_timer.Start();
	}

	editable_local_player->setRotationPitch(updated_pitch + pitchrandsmooth);
	editable_local_player->setPrevRotationPitch(updated_pitch + pitchrandsmooth);

	if (!aa::horizontal_only)
	{
		auto updatedPitch = editable_local_player->getRotationPitch();
		editable_local_player->setRotationPitch(updatedPitch + pitchDiff / (15000.f / speed));
		editable_local_player->setPrevRotationPitch(updatedPitch + pitchDiff / (15000.f / speed));
	}
}

}
