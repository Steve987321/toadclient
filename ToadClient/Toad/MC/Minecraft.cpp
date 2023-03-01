#include "pch.h"
#include "Toad/Toad.h"
#include "Toad/MC/Utils/utils.h"
#include "Minecraft.h"

toadll::c_Minecraft::~c_Minecraft()
{
    env->DeleteLocalRef(mcclass);
    if (elbclass != nullptr) env->DeleteGlobalRef(elbclass);
    if (ariclass != nullptr) env->DeleteGlobalRef(ariclass);
}

jclass toadll::c_Minecraft::get_mcclass()
{
    return findclass(curr_client == minecraft_client::Vanilla ? "ave" : "net.minecraft.client.Minecraft");
}

jclass toadll::c_Minecraft::get_entity_living_class()
{
	if (elbclass == nullptr)
	{
        elbclass = (jclass)env->NewGlobalRef(findclass("net.minecraft.entity.EntityLivingBase"));
	}
    return elbclass;
}

std::unique_ptr<toadll::c_ActiveRenderInfo> toadll::c_Minecraft::get_active_render_info()
{
	if (ariclass == nullptr)
	{
        ariclass = (jclass)env->NewGlobalRef(findclass("net.minecraft.client.renderer.ActiveRenderInfo"));
	}
    
    return std::make_unique<c_ActiveRenderInfo>(ariclass);
}


jobject toadll::c_Minecraft::get_mc() const
{
    return env->CallStaticObjectMethod(mcclass, get_static_mid(mcclass, mapping::getMinecraft));
}

jobject toadll::c_Minecraft::get_rendermanager() const
{
    auto mc = get_mc();
    auto ret = env->CallObjectMethod(mc, get_mid(mc, mapping::getRenderManager));
    env->DeleteLocalRef(mc); 
    return ret;
}

std::shared_ptr<toadll::c_Entity> toadll::c_Minecraft::get_localplayer() const
{
    auto mc = this->get_mc();
	auto playermid = env->GetMethodID(env->GetObjectClass(mc), mappings::findName(mapping::getPlayer), mappings::findSig(mapping::getPlayer));
    auto obj = !playermid ? nullptr : std::make_shared<c_Entity>(env->CallObjectMethod(mc, playermid));
    env->DeleteLocalRef(mc);
    return obj;
}

//void toadll::c_Minecraft::disableLightMap() const
//{
//    auto EntityRenderer = env->CallObjectMethod(get_mc(), get_mid(mcclass, mapping::getEntityRenderer));
//    env->CallObjectMethod(EntityRenderer, get_mid(EntityRenderer, mapping::disableLightmap));
//    env->DeleteLocalRef(EntityRenderer);
//}
//
//void toadll::c_Minecraft::enableLightMap() const
//{
//    auto EntityRenderer = env->CallObjectMethod(get_mc(), get_mid(mcclass, mapping::getEntityRenderer));
//    // void 
//    env->CallObjectMethod(EntityRenderer, get_mid(EntityRenderer, mapping::enableLightmap));
//    env->DeleteLocalRef(EntityRenderer);
//}

jobject toadll::c_Minecraft::get_localplayerobj() const
{
    auto playermid = get_mid(mcclass, mapping::getPlayer);
    auto mc = this->get_mc();
    auto obj = env->CallObjectMethod(mc, playermid);
    env->DeleteLocalRef(mc);
    return obj;
}

jobject toadll::c_Minecraft::get_world() const
{
    auto mc = this->get_mc();
	auto worldmid = get_mid(mcclass, mapping::getWorld);
	auto obj = !worldmid ? nullptr : env->CallObjectMethod(mc, worldmid);

    env->DeleteLocalRef(mc);

    return obj;
}

std::vector<std::shared_ptr<toadll::c_Entity>> toadll::c_Minecraft::get_playerList() const
{
    auto world = p_Minecraft->get_world();
    if (!world) return {};

    auto entites = env->CallObjectMethod(world, get_mid(world, mapping::getPlayerEntities));
    env->DeleteLocalRef(world);

    if (!entites)
        return {};

    auto entitesklass = env->GetObjectClass(entites);
    auto to_arraymid = env->GetMethodID(entitesklass, "toArray", "()[Ljava/lang/Object;");
    env->DeleteLocalRef(entitesklass);

    auto entityarray = (jobjectArray)env->CallObjectMethod(entites, to_arraymid);
    env->DeleteLocalRef(entites);
    if (!entityarray)
        return {};

    int size = env->GetArrayLength(entityarray);

    std::vector<std::shared_ptr<c_Entity>> res;
    res.reserve(size);

    for (int i = 0; i < size; i++)
    {
        jobject obj = env->GetObjectArrayElement(entityarray, i);
        res.emplace_back(std::make_shared<c_Entity>(obj));
    }

    env->DeleteLocalRef(entityarray);

    return res;
}

