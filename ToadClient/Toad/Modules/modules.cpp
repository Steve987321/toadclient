#include "pch.h"
#include "Toad/Toad.h"
#include "modules.h"

void toadll::modules::update()
{
	aa();
	esp();
	edge_sneak();
}

void toadll::modules::aa()
{
	if (!toadll::aa::enabled) return;

	if (GetAsyncKeyState(aa::key))
	{
		std::vector <std::pair<float, std::shared_ptr<c_Entity>>> distances = {};
		auto lPlayer = p_Minecraft->get_localplayer();
		auto lPlayername = lPlayer->get_name();

		for (const auto& player : p_Minecraft->get_playerList())
		{
			if (player->get_name() == lPlayername) continue;
			distances.emplace_back(lPlayer->get_position().dist(player->get_position()), player);
		}
		if (distances.size() < 2) return;
		auto t = std::min_element(distances.begin(), distances.end());
		auto target = t->second;

		auto [yaw, pitch] = get_angles(lPlayer->get_position(), target->get_position());

		auto lyaw = lPlayer->get_rotationYaw();
		auto lpitch = lPlayer->get_rotationPitch();

		float difference = wrap_to_180(-(lyaw - yaw));
		float difference2 = wrap_to_180(-(lpitch - pitch));

		lPlayer->set_rotation(lyaw + difference / 10, lpitch + difference2 / 10);
	}
}

void toadll::modules::esp()
{
}

void toadll::modules::edge_sneak()
{
	// check if enabled 

	auto world = p_Minecraft->get_world();
	if (world == nullptr) return;

	auto mc = p_Minecraft->get_mc();

	auto lplayer = p_Minecraft->get_localplayer();
	auto lplayerpos = lplayer->get_position();
	

	auto lookatobj = env->CallObjectMethod(mc, get_mid(mc, mapping::getObjectMouseOver));
	auto objInfo = jstring2string((jstring)env->CallObjectMethod(lookatobj, env->GetMethodID(env->GetObjectClass(lookatobj), "toString", "()Ljava/lang/String;")));

	std::cout << objInfo << std::endl;

	auto blokpoz = env->CallObjectMethod(lookatobj, get_mid(lookatobj, mapping::getBlockPos));

	auto jo = to_vec3i(blokpoz);

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
		jo.z += 1;
		isEdge = true;
	}
	else if (jo.z < lplayerpos.z && abs(jo.z - lplayerpos.z) > 1)
	{
		jo.z -= 1;
		isEdge = true;
	}


	jclass vec3iClass = findclass("net.minecraft.util.BlockPos");
	auto vec3i = env->NewObject(vec3iClass, env->GetMethodID(vec3iClass, "<init>", "(III)V"), (int)jo.x, (int)jo.y, (int)jo.z);
	if (env->CallBooleanMethod(world, get_mid(world, mapping::isAirBlock), vec3i))
		std::cout << "air block next\n";
	log_Debug("pos{x=%f, y=%f, z=%f} isairblock %s edge %s", jo.x, jo.y, jo.z, env->CallBooleanMethod(world, get_mid(world, mapping::isAirBlock), vec3i) ? "yes" : "no", isEdge ? "yes" : "no");
	Sleep(10);

	env->DeleteLocalRef(lookatobj);
	env->DeleteLocalRef(mc);
	env->DeleteLocalRef(vec3i);
	env->DeleteLocalRef(vec3iClass);
	env->DeleteLocalRef(world);
	env->DeleteLocalRef(blokpoz);
}
