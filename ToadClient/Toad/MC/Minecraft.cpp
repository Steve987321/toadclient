#include "pch.h"
#include "Toad/Toad.h"
#include "Toad/MC/Utils/utils.h"
#include "Minecraft.h"

bool toadll::c_Minecraft::init()
{
	mcclass = findclass(curr_client == minecraft_client::Vanilla ? "ave" : "net.minecraft.client.Minecraft");
	return mcclass != nullptr;
}

jobject toadll::c_Minecraft::get_mc() const
{
	return env->CallStaticObjectMethod(mcclass, get_static_mid(mcclass, mapping::getMinecraft));
}

jobject toadll::c_Minecraft::get_localplayer() const
{
	auto playermid = get_mid(this->mcclass, mapping::getPlayer);
	return env->CallObjectMethod(this->get_mc(), playermid);
}

void toadll::c_Minecraft::clean_up()
{
	env->DeleteLocalRef(mcclass);
}
