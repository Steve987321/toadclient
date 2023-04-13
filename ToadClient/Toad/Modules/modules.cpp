#include "pch.h"
#include "Toad/Toad.h"
#include "modules.h"

std::once_flag flag;

void toadll::modules::update()
{
	auto player = p_Minecraft->get_localplayer();
	if (player == nullptr) return;

	//auto klass = p_Minecraft->get_active_render_class();
	//auto vec = to_vec3(env->CallStaticObjectMethod(klass, get_static_mid(klass, mapping::getRenderPos)));
	//p_Log->LogToConsole("%f, %f, %f", vec.x, vec.y, vec.z);
	//log_Debug("%f, %f, %f", vec.x, vec.y, vec.z);
	
	/*std::cout << env->GetStaticFieldID(klass, "MODELVIEW", "Ljava/nio/FloatBuffer;") << std::endl;
	std::cout << env->GetStaticFieldID(klass, "VIEWPORT", "Ljava/nio/IntBuffer;") << std::endl;
	std::cout << env->GetStaticFieldID(klass, "PROJECTION", "Ljava/nio/FloatBuffer;") << std::endl;*/

	//std::call_once(flag, []
	//	{
	//		auto mc = p_Minecraft->get_mc();
	//		auto obj = env->CallObjectMethod(mc, get_mid(mc, mapping::getGameSettings));
	//		auto objklass = env->GetObjectClass(obj);
	//		std::cout << env->GetFieldID(objklass, "fovSetting", "F") << std::endl;
	//		std::cout << env->GetFloatField(obj, env->GetFieldID(objklass, "fovSetting", "F"));
	//		//loop_through_class(objklass);
	//		env->DeleteLocalRef(mc);
	//		env->DeleteLocalRef(obj);
	//		env->DeleteLocalRef(objklass);
	//	});

	float partial_tick = p_Minecraft->get_partialTick();

	CAimAssist::get_instance()->Update(player, partial_tick);
	CVelocity::get_instance()->Update(player, partial_tick);
	auto_bridge(player);
	CEsp::get_instance()->Update(player, partial_tick);
	//update_esp_vars(player);

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

//void toadll::modules::velocity(const std::shared_ptr<c_Entity>& lPlayer)
//{
//	if (!velocity::enabled) return;
//
//	static bool once = false;
//
//	if (int hurttime = lPlayer->get_hurt_time(); hurttime > 0 && !once)
//	{
//		if (toad::rand_int(0, 100) > velocity::chance) { once = true; return; }
//
//		if (velocity::delay > 0) toad::preciseSleep(velocity::delay * 0.05f);
//
//		auto motionX = lPlayer->get_motionX();
//		auto newMotionX = std::lerp(motionX, motionX * (velocity::horizontal / 100.f), 0.3f);
//		auto motionZ = lPlayer->get_motionZ();
//		auto newMotionZ = std::lerp(motionZ, motionZ * (velocity::horizontal / 100.f), 0.3f);
//
//		if (abs(motionX) > 0)
//			lPlayer->set_motionX(newMotionX);
//		if (abs(motionZ) > 0)
//			lPlayer->set_motionZ(newMotionZ);
//
//		// TODO: separate horizontal and vertical velocity module in separate threads? 
//
//		if (lPlayer->get_motionY() > 0) // normal velocity when going down 
//			lPlayer->set_motionY(lPlayer->get_motionY() * (velocity::vertical / 100.f));
//
//		if (velocity::vertical > 0 && velocity::horizontal > 0)
//			toad::preciseSleep(50.0 / 1000);
//		
//	}
//	else if (hurttime <= 0)
//	{
//		once = false;
//	}
//}

//void toadll::modules::update_esp_vars(const std::shared_ptr<c_Entity>& lPlayer)
//{
//	//if (!EntityEsp::enabled) return;
//
//	static auto ari = p_Minecraft->get_active_render_info();
//	auto playerList = p_Minecraft->get_playerList();
//
//	renderNames.resize(playerList.size(), {{-1, -1}, nullptr});
//
//	for (int i = 0; i < playerList.size(); i++)
//	{
//		const auto& e = playerList[i];
//		if (env->IsSameObject(lPlayer->obj, e->obj)) continue;
//
//		vec2 screenposition{0,0};
//		vec3 realPos = lPlayer->get_position() + ari->get_render_pos();
//		vec2 viewangles{lPlayer->get_rotationYaw(), lPlayer->get_rotationPitch()};
//
//		vec3 ePos = e->get_position();
//		float yawDiff = abs(wrap_to_180(-(lPlayer->get_rotationYaw() - get_angles(lPlayer->get_position(), ePos).first)));
//		//p_Log->LogToConsole(std::to_string(yawDiff));
//
//		if (yawDiff > 110) continue; // will not render anyway
//
//		if (WorldToScreen(realPos, ePos, viewangles, p_Minecraft->get_fov(), screenposition))
//		{
//			renderNames[i] = std::make_pair(screenposition, e->get_name().c_str());
//		}
//	}
//}

INPUT ip{};
std::once_flag onceFlag;
void toadll::modules::auto_bridge(const std::shared_ptr<c_Entity>& lPlayer)
{
	//if (!auto_bridge::enabled || is_cursor_shown) return;
	
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

	static bool flag = false;

	// extra checks and settings
	float rotPitch = lPlayer->get_rotationPitch();
	if (rotPitch < auto_bridge::pitch_check) return;

	//good for break blocks
	auto theMC = p_Minecraft->get_mc();
	auto lookatobj = env->CallObjectMethod(theMC, get_mid(theMC, mapping::getObjectMouseOver));
	auto blokpoz = env->CallObjectMethod(lookatobj, get_mid(lookatobj, mapping::getBlockPos));

	jmethodID med = get_mid(theMC, mapping::getObjectMouseOver);
	printf("get object mouse over %p %d, %lu", med, med, med);

	env->DeleteLocalRef(theMC);
	
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
