#include "pch.h"
#include "Toad/Toad.h"
#include "aimassist.h"

using namespace toad;

namespace toadll {

void CAimAssist::Update(const std::shared_ptr<LocalPlayerT>& lPlayer)
{
	//std::cout << "AA, enabled, cursor shown, alwasy aim , mdown :" << aa::enabled << " " << is_cursor_shown << " " << aa::always_aim << " " << static_cast<bool>(GetAsyncKeyState(VK_LBUTTON)) << std::endl;
	if (!aa::enabled || CVarsUpdater::IsInGui)
	{
		SLOW_SLEEP(50);
		return;
	}
	if (!aa::always_aim && !GetAsyncKeyState(VK_LBUTTON))
	{
		SLOW_SLEEP(10);
		return;
	}

	std::vector <std::pair<float, EntityT>> distances = {};

	static std::shared_ptr<EntityT> target = nullptr;
	float speed = aa::speed;
	float minimalAngleDiff = aa::fov / 2.f;

	if (aa::lock_aim && target != nullptr)
	{
		if  (
			abs(wrap_to_180(-(lPlayer->Yaw - get_angles(lPlayer->Pos, target->Pos).first))) <= aa::fov 
			&&
			target->Pos.dist(lPlayer->Pos) <= aa::distance
			)
			goto AIMING;
	}

	target = nullptr; 

	//get a target
	for (const auto& player : CVarsUpdater::GetPlayerList())
	{
		if (lPlayer->Pos.dist(player.Pos) > aa::distance) continue;
		if (player.Invis && !aa::invisibles) continue;

		distances.emplace_back(lPlayer->Pos.dist(player.Pos), player);
		if (aa::targetFOV)
		{
			float yawDiff = abs(wrap_to_180(-(lPlayer->Yaw - get_angles(lPlayer->Pos, player.Pos).first)));
			if (yawDiff < minimalAngleDiff)
			{
				minimalAngleDiff = yawDiff;
				target = std::make_shared<EntityT>(player);
			}
		}
	}
	if (distances.empty()) return; // atleast one other player

	// getting target by distance
	if (!aa::targetFOV)
	{
		auto t = std::ranges::min_element(distances, [](const auto & l, const auto & r) { return l.first < r.first; });
		target = std::make_shared<EntityT>(t->second);
	}

AIMING:
	if (target == nullptr) return;

	auto targetPos = target->Pos;
	auto lplayerPos = lPlayer->Pos;
	vec3 aimPoint;

	const std::vector<vec3> bboxCorners
	{
		{ targetPos.x - 0.3f, targetPos.y, targetPos.z + 0.3f },
		{ targetPos.x - 0.3f, targetPos.y, targetPos.z - 0.3f },
		{ targetPos.x + 0.3f, targetPos.y, targetPos.z - 0.3f },
		{ targetPos.x + 0.3f, targetPos.y, targetPos.z + 0.3f },
	};

	if (aa::aim_at_closest_point) // aims at the closest corner of target
	{
		const std::vector<float> distances = {
			bboxCorners.at(0).dist(lplayerPos),
			bboxCorners.at(1).dist(lplayerPos),
			bboxCorners.at(2).dist(lplayerPos),
			bboxCorners.at(3).dist(lplayerPos),
		};

		auto closestcorner = bboxCorners[std::distance(distances.begin(), std::ranges::min_element(distances))];

		aimPoint = closestcorner;
	}
	else // aims to target if players aim is not inside hitbox 
	{
		auto lplayeryaw = lPlayer->Yaw;
		auto yawdiffToPos = wrap_to_180(-(lplayeryaw - get_angles(lplayerPos, targetPos).first));

		const std::vector<float> yawdiffs = {
			wrap_to_180(-(lplayeryaw - get_angles(lplayerPos, bboxCorners.at(0)).first)),
			wrap_to_180(-(lplayeryaw - get_angles(lplayerPos, bboxCorners.at(1)).first)),
			wrap_to_180(-(lplayeryaw - get_angles(lplayerPos, bboxCorners.at(2)).first)),
			wrap_to_180(-(lplayeryaw - get_angles(lplayerPos, bboxCorners.at(3)).first)),
		};

		if (yawdiffToPos < 0)
		{
			if (*std::ranges::max_element(yawdiffs) > 0)
				return;
		}
		else
		{
			if (*std::ranges::min_element(yawdiffs) < 0)
				return;
		}

		aimPoint = target->Pos;
	}

	auto [yawtarget, pitchtarget] = get_angles(lPlayer->Pos, aimPoint/*target->get_position()*/);

	auto lyaw = lPlayer->Yaw;
	auto lpitch = lPlayer->Pitch;

	float yawDiff = wrap_to_180(-(lyaw - yawtarget));
	float absYawDiff = abs(yawDiff);
	if (absYawDiff < 3.f) return;
	float pitchDiff = wrap_to_180(-(lpitch - pitchtarget));

	if (!aa::targetFOV) // don't have to check if this is enabled because already checked
		if (absYawDiff > minimalAngleDiff)
			return;
	// got target and yaw and pitch offsets

	yawDiff += toadll::rand_float(-2.f, 2.f);
	pitchDiff += toadll::rand_float(-2.f, 2.f);

	const int rand_100 = toadll::rand_int(0, 100);

	static CTimer speed_rand_timer;
	static float reaction_time_timer = 0;
	static float long_speed_modifier = 1;
	static float prev_long_speed_modifier = 1;
	static float long_speed_modifier_smooth = 1;

	float smooth = speed;

	speed = std::lerp(speed, smooth, 0.05f);

	if (speed_rand_timer.Elapsed<>() >= 100)
	{
		prev_long_speed_modifier = long_speed_modifier;
		long_speed_modifier = toadll::rand_float(0.7f, 1.3f);
		speed_rand_timer.Start();
		//std::cout << "reset :" << long_speed_modifier << std::endl;
	}

	long_speed_modifier_smooth = slerp(prev_long_speed_modifier, long_speed_modifier, speed_rand_timer.Elapsed<>() / 100);
	static float yawdiffSpeed = 0;

	if (static bool once = false; !once || reaction_time_timer > aa::reaction_time)
	{
		yawdiffSpeed = yawDiff / (15000.f / speed * long_speed_modifier_smooth);
		once = true;
		reaction_time_timer = 0;
	}

	auto EditableLocalPlayer = Minecraft->getLocalPlayer();
	if (!EditableLocalPlayer)
		return;

	auto updatedYaw = EditableLocalPlayer->getRotationYaw();
	//log_Debug("%s | %f = %f + %f", target->get_name().c_str(), lyaw + yawdiffSpeed, lyaw, yawdiffSpeed);
	EditableLocalPlayer->setRotationYaw(updatedYaw + yawdiffSpeed);
	EditableLocalPlayer->setPrevRotationYaw(updatedYaw + yawdiffSpeed);

	// pitch randomization
	static CTimer pitch_rand_timer;
	static float pitchrand = rand_float(-0.0025f, 0.0025f);
	static float pitchupdatems = rand_float(100, 200);
	static float pitchrandsmooth = 0;
	static float pitchrandbegin = 0;
	auto updatedPitch = EditableLocalPlayer->getRotationPitch();
	if (pitch_rand_timer.Elapsed<>() > pitchupdatems)
	{
		pitchrandbegin = pitchrand;
		pitchupdatems = rand_float(100, 200);
		pitchrand = rand_float(-0.0015f, 0.0015f);
		pitch_rand_timer.Start();
	}

	pitchrandsmooth = slerp(pitchrandbegin, pitchrand, pitch_rand_timer.Elapsed<>() / pitchupdatems);

	if (rand_100 < 10)
		pitchrandsmooth += rand_float(-0.001f, 0.001f);

	EditableLocalPlayer->setRotationPitch(updatedPitch + pitchrandsmooth);
	EditableLocalPlayer->setPrevRotationPitch(updatedPitch + pitchrandsmooth);

	if (!aa::horizontal_only)
	{
		auto updatedPitch = EditableLocalPlayer->getRotationPitch();
		EditableLocalPlayer->setRotationPitch(updatedPitch + pitchDiff / (15000.f / speed));
		EditableLocalPlayer->setPrevRotationPitch(updatedPitch + pitchDiff / (15000.f / speed));
	}

	toadll::preciseSleep(toadll::rand_float(0.0001f, 0.0005f)); // 1-5ms
	reaction_time_timer += CVarsUpdater::PartialTick;
}

}
