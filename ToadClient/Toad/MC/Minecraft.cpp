#include "pch.h"
#include "Toad/Toad.h"
#include "Toad/MC/Utils/utils.h"
#include "Minecraft.h"

bool toadll::c_Minecraft::init()
{
	mcclass = findclass(curr_client == minecraft_client::Vanilla ? "ave" : "net.minecraft.client.Minecraft");
	return mcclass != nullptr;
}

jclass toadll::c_Minecraft::get_mcclass() const
{
    return this->mcclass;
}

jobject toadll::c_Minecraft::get_mc() const
{
	return env->CallStaticObjectMethod(mcclass, get_static_mid(mcclass, mapping::getMinecraft));
}

jobject toadll::c_Minecraft::get_rendermanager() const
{
    return env->CallObjectMethod(get_mc(), get_mid(mcclass, mapping::getRenderManager));
}

std::shared_ptr<toadll::c_Entity> toadll::c_Minecraft::get_localplayer() const
{
	auto playermid = env->GetMethodID(this->mcclass, mappings::findName(mapping::getPlayer), mappings::findSig(mapping::getPlayer));
    return !playermid ? nullptr : std::make_shared<c_Entity>(std::make_shared<jobject>(env->CallObjectMethod(this->get_mc(), playermid)));
}

jobject toadll::c_Minecraft::get_localplayerobj() const
{
    auto playermid = get_mid(this->mcclass, mapping::getPlayer);
    return env->CallObjectMethod(this->get_mc(), playermid);
}

jobject toadll::c_Minecraft::get_world() const
{
	auto worldmid = get_mid(this->mcclass, mapping::getWorld);
	return !worldmid ? nullptr : env->CallObjectMethod(this->get_mc(), worldmid);
}

std::vector<std::shared_ptr<toadll::c_Entity>> toadll::c_Minecraft::get_playerList() const
{
    auto theWorld = get_world();
    if (theWorld == nullptr) return {};

    auto getPlayerEtitiesMid = get_mid(env->GetObjectClass(theWorld), mapping::getPlayerEntities);

    auto entites = env->CallObjectMethod(get_world(), getPlayerEtitiesMid);

    auto to_arraymid = env->GetMethodID(env->GetObjectClass(entites), "toArray", "()[Ljava/lang/Object;");
    auto entityarray = reinterpret_cast<jobjectArray>(env->CallObjectMethod(entites, to_arraymid));
    int size = env->GetArrayLength(entityarray);

    std::vector<std::shared_ptr<c_Entity>> res;
    res.reserve(size);

    for (int i = 0; i < size; i++)
    {
        res.emplace_back(std::make_shared<c_Entity>(std::make_shared<jobject>(env->GetObjectArrayElement(entityarray, i))));
    }

    env->DeleteLocalRef(entites);
    env->DeleteLocalRef(entityarray);

    return res;
}

void toadll::c_Minecraft::clean_up()
{
	env->DeleteLocalRef(mcclass);
}
