#include "pch.h"
#include "Toad/Toad.h"
#include "modules.h"

void toadll::modules::update()
{
	auto player = p_Minecraft->get_localplayer();
	if (player == nullptr) return;
	 
	aa(player);

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

void toadll::modules::aa(const std::shared_ptr<c_Entity>& lPlayer)
{
	if (!aa::enabled || is_cursor_shown) return;
	
	if (GetAsyncKeyState(aa::key))
	{
		std::vector <std::pair<float, std::shared_ptr<c_Entity>>> distances = {};

		for (const auto& player : p_Minecraft->get_playerList())
		{
			if (env->IsSameObject(lPlayer->obj, player->obj)) continue;
			if (lPlayer->get_position().dist(player->get_position()) > aa::distance) continue;
			distances.emplace_back(lPlayer->get_position().dist(player->get_position()), player);
		}
		if (distances.empty()) return; // atleast one other player

		auto t = std::min_element(distances.begin(), distances.end());

		//if (t->first > aa::distance) return;

		auto& target = t->second;
		auto [yaw, pitch] = get_angles(lPlayer->get_position(), target->get_position());

		auto lyaw = lPlayer->get_rotationYaw();
		auto lpitch = lPlayer->get_rotationPitch();

		float yawDiff = wrap_to_180(-(lyaw - yaw));
		float pitchDiff = wrap_to_180(-(lpitch - pitch));

		yawDiff += toad::rand_float(-1.f, 1.f);
		pitchDiff += toad::rand_float(-2.f, 2.f);

		lPlayer->set_rotationYaw(lyaw + yawDiff / (10000.f / aa::speed));
		lPlayer->set_prevRotationYaw(lyaw + yawDiff / (10000.f / aa::speed));

		if (!aa::horizontal_only)
		{
			lPlayer->set_rotationPitch(lpitch + pitchDiff / (10000.f / aa::speed));
			lPlayer->set_prevRotationPitch(lpitch + pitchDiff / (10000.f / aa::speed));
		}

		//lPlayer->set_rotation(lyaw + yawDiff / (10000.f / aa::speed), lpitch + pitchDiff / (10000.f / aa::speed));
	}
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
