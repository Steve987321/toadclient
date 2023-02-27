#include "pch.h"
#include "Toad/Toad.h"
#include "modules.h"

void toadll::modules::update()
{
	auto player = p_Minecraft->get_localplayer();
	if (player == nullptr) return;

	velocity(player);
	aa(player);
	auto_bridge(player);

	/*auto heldItem = lPlayer->get_heldItem();
	if (heldItem != NULL)
	{
		log_Debug("%s", jstring2string(jstring(env->CallObjectMethod(heldItem, env->GetMethodID(env->GetObjectClass(heldItem), "toString", "()Ljava/lang/String;")))).c_str());
		env->DeleteLocalRef(heldItem);
	}
	else
	{
		log_Debug("no");
	}*/

	//aa();
	//auto_bridge();

}

void toadll::modules::velocity(const std::shared_ptr<c_Entity>& lPlayer)
{
	if (!velocity::enabled) return;

	static bool once = false;

	if (lPlayer->get_hurt_time() > 0 && !once)
	{
		once = true;
		/*log_Debug("hurt Time %d, motionxyz(x:%.3f, %.3f, %.3f), new motionxyz(x:%.3f, y:%.3f, z:%.3f)",
			lPlayer->get_hurt_time(),
			lPlayer->get_motionX(), lPlayer->get_motionY(), lPlayer->get_motionZ(),
			lPlayer->get_motionX() * (velocity::horizontal / 100.f),
			lPlayer->get_motionY() * (velocity::horizontal / 100.f),
			lPlayer->get_motionZ() * (velocity::vertical / 100.f)
		);*/

		if (once)
		{
			if (toad::rand_int(0, 100) >= velocity::chance) return;
		}

		if (velocity::delay > 0) toad::preciseSleep(velocity::delay / 100.f);

		if (abs(lPlayer->get_motionX()) > 0)
			lPlayer->set_motionX(lPlayer->get_motionX() * (velocity::horizontal / 100.f));
		if (abs(lPlayer->get_motionZ()) > 0)
			lPlayer->set_motionZ(lPlayer->get_motionZ() * (velocity::horizontal / 100.f));
		if (lPlayer->get_motionY() > 0) // normal velocity when going down 
			lPlayer->set_motionY(lPlayer->get_motionY() * (velocity::vertical / 100.f));
	}
	else
		once = false;
}

void toadll::modules::aa(const std::shared_ptr<c_Entity>& lPlayer)
{
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

	if (!aa::targetFOV)
	{
		auto t = std::ranges::min_element(distances);
		target = t->second;
	}
	if (target == nullptr) return;

	auto [yaw, pitch] = get_angles(lPlayer->get_position(), target->get_position());

	auto lyaw = lPlayer->get_rotationYaw();
	auto lpitch = lPlayer->get_rotationPitch();

	float yawDiff = wrap_to_180(-(lyaw - yaw));
	float absYawDiff = abs(yawDiff);
	if (absYawDiff < 1) return;
	float pitchDiff = wrap_to_180(-(lpitch - pitch));

	if (!aa::targetFOV) // don't have to check if this is enabled because already checked
		if (absYawDiff > minimalAngleDiff)
			return;

	yawDiff += toad::rand_float(-2.f, 2.f);
	pitchDiff += toad::rand_float(-2.f, 2.f);
	float smooth = speed;

	if (absYawDiff > 7)
	{
		smooth *= toad::rand_float(0.4f, 2.0f);
	}
	else if (absYawDiff < 7)
	{
		smooth *= toad::rand_float(0.0f, 0.4f);
	}
	speed = std::lerp(speed, smooth, 0.3f);

	auto yawdiffSpeed = yawDiff / (15000.f / speed);

	if (toad::rand_int(0, 2) == 1)
	{
		yawdiffSpeed += toad::rand_float(-0.005f, 0.005f);
	}
	lPlayer->set_rotationYaw(lyaw + yawdiffSpeed);
	lPlayer->set_prevRotationYaw(lyaw + yawdiffSpeed);
	if (toad::rand_int(0, 3) == 1) {
		float pitchrand = toad::rand_float(-0.005f, 0.005f);
		lPlayer->set_rotationPitch(lpitch + pitchrand);
		lPlayer->set_prevRotationPitch(lpitch + pitchrand);
	}
	
	if (!aa::horizontal_only)
	{
		lPlayer->set_rotationPitch(lpitch + pitchDiff / (15000.f / speed));
		lPlayer->set_prevRotationPitch(lpitch + pitchDiff / (15000.f / speed));
	}
	toad::preciseSleep(toad::rand_float(0.001 / 1000, 0.2 / 1000));

	//lPlayer->set_rotation(lyaw + yawDiff / (10000.f / aa::speed), lpitch + pitchDiff / (10000.f / aa::speed));

}

void toadll::modules::esp(const vec3& ePos)
{
	
	
}

INPUT ip{};
std::once_flag onceFlag;
void toadll::modules::auto_bridge(const std::shared_ptr<c_Entity>& lPlayer)
{
	if (!auto_bridge::enabled || is_cursor_shown) return;

	// define our ip struct
	std::call_once(onceFlag, []
	{
			ip.type = INPUT_KEYBOARD;
			ip.ki.wScan = 0; 
			ip.ki.time = 0;
			ip.ki.dwExtraInfo = 0;

			ip.ki.wVk = VK_SHIFT; 
		});

	auto lplayerpos = lPlayer->get_position();

	//auto lplayer = p_Minecraft->get_localplayer();
	/*auto blockpos = env->CallObjectMethod(*lplayer->obj, get_mid(*lplayer->obj, mapping::getBlockPosition));
	auto vec3 = to_vec3i(blockpos);*/
	//auto vec3 = lplayer->get_position();
	//vec3.x = floor(vec3.x);
	//vec3.y = floor(vec3.y);
	//vec3.z = floor(vec3.z);
	//log_Debug("vec3: (%f %f %f) absDist: %f", vec3.x, vec3.y, vec3.z, abs(vec3.dist(lplayer->get_position())));

	static bool flag = false;

	// extra checks and settings
	float rotPitch = p_Minecraft->get_localplayer()->get_rotationPitch();
	if (rotPitch < auto_bridge::pitch_check) return;

	//good for break blocks
	auto lookatobj = env->CallObjectMethod(p_Minecraft->get_mc(), get_mid(p_Minecraft->get_mc(), mapping::getObjectMouseOver));
	auto blokpoz = env->CallObjectMethod(lookatobj, get_mid(lookatobj, mapping::getBlockPos));

	// no dec
	auto jo = to_vec3i(blokpoz);

	env->DeleteLocalRef(blokpoz);
	env->DeleteLocalRef(lookatobj);

	if (jo.y != floor(lplayerpos.y) - 1)
		return;

	bool isEdge = false;
	if (jo.x > lplayerpos.x && abs(jo.x - lplayerpos.x) > 0.1f)
	{
		jo.x -= 1;
		isEdge = true;
	}
	else if (jo.x < lplayerpos.x && abs(jo.x - lplayerpos.x) > 1)
	{
		jo.x += 1;
		isEdge = true;
	}
	if (jo.z > lplayerpos.z && abs(jo.z - lplayerpos.z) > 0.1f)
	{
		jo.z -= 1;
		isEdge = true;
	}
	else if (jo.z < lplayerpos.z && abs(jo.z - lplayerpos.z) > 1)
	{
		jo.z += 1;
		isEdge = true;
	}

	auto vec3iClass = findclass(curr_client == minecraft_client::Vanilla ? "cj" : "net.minecraft.util.Vec3i");
	if (vec3iClass == nullptr)
		return;
	

	auto vec3i = env->NewObject(vec3iClass, env->GetMethodID(vec3iClass, "<init>", "(III)V"), (int)jo.x, (int)jo.y, (int)jo.z);
	env->DeleteLocalRef(vec3iClass);
	auto world = p_Minecraft->get_world();

	if (env->CallBooleanMethod(world, get_mid(world, mapping::isAirBlock), vec3i) && isEdge)
	{
		if (abs(jo.y - lplayerpos.y) == 2)
		{
			ip.ki.dwFlags = 0; // 0 for key press
			SendInput(1, &ip, sizeof(INPUT));
			flag = true;
		}

		if (!flag)
		{
			//std::cout << "down\n";
			
			ip.ki.dwFlags = 0; // 0 for key press
			SendInput(1, &ip, sizeof(INPUT));

			//PostMessage(GetForegroundWindow(), WM_KEYDOWN, VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC));
			
			SLOW_SLEEP(7);

			flag = !flag;
		}
	}
	else
	{
		if (flag)
		{
				
			ip.ki.dwFlags = KEYEVENTF_KEYUP; // 0 for key press
			SendInput(1, &ip, sizeof(INPUT));

			//keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
			//PostMessage(GetForegroundWindow(), WM_KEYUP, VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC));
			SLOW_SLEEP(7);
			flag = !flag;
		}
	}

	env->DeleteLocalRef(world);
	env->DeleteLocalRef(vec3i);

	//log_Debug("pos{x=%f, y=%f, z=%f} isairblock %s edge %s", jo.x, jo.y, jo.z, env->CallBooleanMethod(world, get_mid(world, mapping::isAirBlock), vec3i) ? "yes" : "no", isEdge ? "yes" : "no");

	//Sleep(1);
}
