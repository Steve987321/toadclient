#include "pch.h"
#include "Toad/Toad.h"
#include "aimassist.h"

namespace toadll {

void CAimAssist::Update(const std::shared_ptr<c_Entity>& lPlayer, float partialTick)
{
	//std::cout << "AA, enabled, cursor shown, alwasy aim :" << aa::enabled << " " << is_cursor_shown << " " << aa::always_aim << std::endl;
	if (!aa::enabled || is_cursor_shown) return;
	if (!aa::always_aim && !GetAsyncKeyState(VK_LBUTTON)) return;

	std::vector <std::pair<float, std::shared_ptr<c_Entity>>> distances = {};

	std::shared_ptr<c_Entity> target = nullptr;

	float speed = aa::speed;
	float minimalAngleDiff = aa::fov / 2.f;

	for (const auto& player : p_Minecraft->get_playerList())
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

	auto [yawtarget, pitchtarget] = get_angles(lPlayer->get_position(), target->get_position());

	auto lyaw = lPlayer->get_rotationYaw();
	auto lpitch = lPlayer->get_rotationPitch();

	float yawDiff = wrap_to_180(-(lyaw - yawtarget));
	float absYawDiff = abs(yawDiff);
	if (auto stopaiming = aa::auto_aim ? 3.f : 1.f; absYawDiff < stopaiming) return;
	float pitchDiff = wrap_to_180(-(lpitch - pitchtarget));

	if (!aa::targetFOV) // don't have to check if this is enabled because already checked
		if (absYawDiff > minimalAngleDiff)
			return;
	// got target and yaw and pitch offsets

	yawDiff += toad::rand_float(-2.f, 2.f);
	pitchDiff += toad::rand_float(-2.f, 2.f);

	const int rand_100 = toad::rand_int(0, 100);

	static float speed_rand_timer = 200;
	static float long_speed_modifier = 1;
	static float long_speed_modifier_smooth = 1;

	float smooth = speed;

	if (absYawDiff > 7)
	{
		smooth *= aa::auto_aim ? toad::rand_float(2.0f, 3.0f) : toad::rand_float(0.4f, 2.0f);
	}
	else if (absYawDiff < 7)
	{
		smooth *= aa::auto_aim ? toad::rand_float(0.5f, 1.f) : toad::rand_float(0.0f, 0.4f);
	}

	speed = std::lerp(speed, smooth, aa::auto_aim ? 0.05f : 0.3f);

	if (speed_rand_timer < 0)
	{
		long_speed_modifier = toad::rand_float(0.7f, 1.3f);
		speed_rand_timer = 200;
		//std::cout << "reset :" << long_speed_modifier << std::endl;
	}

	long_speed_modifier_smooth = std::lerp(long_speed_modifier_smooth, long_speed_modifier, 0.05f);

	static float reaction_time_timer = aa::reaction_time + 1;

	if (static bool once = false; !once || reaction_time_timer > aa::reaction_time)
	{
		yawdiffSpeed = yawDiff / (15000.f / speed * long_speed_modifier_smooth);

		once = true;
		reaction_time_timer = 0;
	}

	if (toad::rand_int(0, aa::auto_aim ? 10 : 2) == 1)
	{
		yawdiffSpeed += aa::auto_aim ? toad::rand_float(-0.01f, 0.01f) : toad::rand_float(-0.005f, 0.005f);
	}

	lPlayer->set_rotationYaw(lyaw + yawdiffSpeed);
	lPlayer->set_prevRotationYaw(lyaw + yawdiffSpeed);

	if (rand_100 <= 10) {
		float pitchrand = toad::rand_float(-0.005f, 0.005f);
		lPlayer->set_rotationPitch(lpitch + pitchrand);
		lPlayer->set_prevRotationPitch(lpitch + pitchrand);
	}

	if (!aa::horizontal_only)
	{
		lPlayer->set_rotationPitch(lpitch + pitchDiff / (15000.f / speed));
		lPlayer->set_prevRotationPitch(lpitch + pitchDiff / (15000.f / speed));
	}
	toad::preciseSleep(
		aa::auto_aim
		? toad::rand_float(0.0001f, 0.0005f)
		: toad::rand_float(0.001f / 1000.f, 0.2f / 1000.f)
	);

	speed_rand_timer -= partialTick;
	reaction_time_timer += partialTick;
}

}
