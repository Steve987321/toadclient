#include "pch.h"
#include "Toad/Toad.h"
#include "aimassist.h"

using namespace toad;

namespace toadll {

void CAimAssist::Update(const std::shared_ptr<c_Entity>& lPlayer)
{
	//std::cout << "AA, enabled, cursor shown, alwasy aim , mdown :" << aa::enabled << " " << is_cursor_shown << " " << aa::always_aim << " " << static_cast<bool>(GetAsyncKeyState(VK_LBUTTON)) << std::endl;
	if (!aa::enabled || g_is_cursor_shown)
	{
		SLOW_SLEEP(10);
		return;
	}
	if (!aa::always_aim && !GetAsyncKeyState(VK_LBUTTON))
	{
		SLOW_SLEEP(10);
		return;
	}

	std::vector <std::pair<float, std::shared_ptr<c_Entity>>> distances = {};

	static std::shared_ptr<c_Entity> target = nullptr;
	float speed = aa::speed;
	float minimalAngleDiff = aa::fov / 2.f;

	if (aa::lock_aim && target != nullptr)
	{
		if  (
			abs(wrap_to_180(-(lPlayer->get_rotationYaw() - get_angles(lPlayer->get_position(), target->get_position()).first))) <= aa::fov 
			&&
			target->get_position().dist(lPlayer->get_position()) <= aa::distance
			)
			goto AIMING;
	}
		

	target = nullptr;

	// get a target
	for (const auto& player : Minecraft->get_playerList())
	{
		if (env->IsSameObject(lPlayer->obj, player->obj)) continue;

		if (lPlayer->get_position().dist(player->get_position()) > aa::distance) continue;
		if (player->is_invisible() && !aa::invisibles) continue;

		distances.emplace_back(lPlayer->get_position().dist(player->get_position()), player);
		if (aa::targetFOV)
		{
			float yawDiff = abs(wrap_to_180(-(lPlayer->get_rotationYaw() - get_angles(lPlayer->get_position(), player->get_position()).first)));
			if (yawDiff < minimalAngleDiff)
			{
				minimalAngleDiff = yawDiff;
				target = player;
			}
		}
	}
	if (distances.empty()) return; // atleast one other player

	// getting target by distance
	if (!aa::targetFOV)
	{
		auto t = std::ranges::min_element(distances);
		target = t->second;
	}
	if (target == nullptr) return;

AIMING:

	auto targetPos = target->get_position();
	auto lplayerPos = lPlayer->get_position();
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
		auto lplayeryaw = lPlayer->get_rotationYaw();
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

		aimPoint = target->get_position();
	}

	auto [yawtarget, pitchtarget] = get_angles(lPlayer->get_position(), aimPoint/*target->get_position()*/);

	auto lyaw = lPlayer->get_rotationYaw();
	auto lpitch = lPlayer->get_rotationPitch();

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

	static float speed_rand_timer = 200;
	static float reaction_time_timer = 0;
	static float long_speed_modifier = 1;
	static float long_speed_modifier_smooth = 1;

	float smooth = speed;

	speed = std::lerp(speed, smooth, 0.05f);

	if (speed_rand_timer < 0)
	{
		long_speed_modifier = toadll::rand_float(0.7f, 1.3f);
		speed_rand_timer = 200;
		//std::cout << "reset :" << long_speed_modifier << std::endl;
	}

	long_speed_modifier_smooth = std::lerp(long_speed_modifier_smooth, long_speed_modifier, 0.05f);
	static float yawdiffSpeed = 0;

	if (static bool once = false; !once || reaction_time_timer > aa::reaction_time)
	{
		yawdiffSpeed = yawDiff / (15000.f / speed * long_speed_modifier_smooth);
		once = true;
		reaction_time_timer = 0;
	}

	//log_Debug("%s | %f = %f + %f", target->get_name().c_str(), lyaw + yawdiffSpeed, lyaw, yawdiffSpeed);
	lPlayer->set_rotationYaw(lyaw + yawdiffSpeed);
	lPlayer->set_prevRotationYaw(lyaw + yawdiffSpeed);

	if (rand_100 <= 10) {
		float pitchrand = toadll::rand_float(-0.005f, 0.005f);
		lPlayer->set_rotationPitch(lpitch + pitchrand);
		lPlayer->set_prevRotationPitch(lpitch + pitchrand);
	}

	if (!aa::horizontal_only)
	{
		lPlayer->set_rotationPitch(lpitch + pitchDiff / (15000.f / speed));
		lPlayer->set_prevRotationPitch(lpitch + pitchDiff / (15000.f / speed));
	}

	toadll::preciseSleep(toadll::rand_float(0.0001f, 0.0005f));
	auto ptick = Minecraft->get_partialTick();
	speed_rand_timer -= ptick;
	reaction_time_timer += ptick;
}

}
