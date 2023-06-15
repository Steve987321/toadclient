#include "pch.h"
#include "Toad/Toad.h"
#include "Toad/MC/Utils/mcutils.h"
#include "Minecraft.h"

toadll::c_Minecraft::~c_Minecraft()
{
    env->DeleteLocalRef(mcclass);
    if (elbclass != nullptr) env->DeleteGlobalRef(elbclass);
    if (ariclass != nullptr) env->DeleteGlobalRef(ariclass);
}

jclass toadll::c_Minecraft::get_mcclass(JNIEnv* env)
{
    return findclass(curr_client == minecraft_client::Vanilla ? "ave" : "net.minecraft.client.Minecraft", env);
}

jclass toadll::c_Minecraft::get_entity_living_class()
{
	if (elbclass == nullptr)
	{
        elbclass = (jclass)env->NewGlobalRef(findclass("net.minecraft.entity.EntityLivingBase", env));
	}
    return elbclass;
}

std::unique_ptr<toadll::c_ActiveRenderInfo> toadll::c_Minecraft::get_active_render_info()
{
	if (ariclass == nullptr)
	{
        ariclass = (jclass)env->NewGlobalRef(findclass("net.minecraft.client.renderer.ActiveRenderInfo", env));
	}
    
    return std::make_unique<c_ActiveRenderInfo>(ariclass, env);
}

 
jobject toadll::c_Minecraft::get_mc()
{
    return env->CallStaticObjectMethod(mcclass, get_static_mid(mcclass, mapping::getMinecraft, env));
}

jobject toadll::c_Minecraft::get_rendermanager()
{
    auto mc = get_mc();
    auto ret = env->CallObjectMethod(mc, get_mid(mc, mapping::getRenderManager, env));
    env->DeleteLocalRef(mc); 
    return ret;
}

std::shared_ptr<toadll::c_Entity> toadll::c_Minecraft::get_localplayer()
{
    auto mc = get_mc();
    auto obj = env->GetObjectField(mc, get_fid(mc, mappingFields::thePlayerField, env));
    //auto playermid = get_mid(mc, mapping::getPlayer, env);
    //auto obj = !playermid ? nullptr : std::make_shared<c_Entity>(env->CallObjectMethod(mc, playermid), env, get_entity_living_class());
    if (env->ExceptionCheck())
    {
        log_Debug("OH NO EXCEPTION!");
        return nullptr;
    }
    env->DeleteLocalRef(mc);
    return std::make_shared<c_Entity>(obj, env, get_entity_living_class());
}

void toadll::c_Minecraft::set_gamma(float val)
{
    auto obj = get_gamesettings();

    env->CallVoidMethod(obj, get_mid(obj, mapping::setGamma, env), val);

    env->DeleteLocalRef(obj);
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

jobject toadll::c_Minecraft::get_localplayerobj()
{
    auto playermid = get_mid(mcclass, mapping::getPlayer, env);
    auto mc = this->get_mc();
    auto obj = env->CallObjectMethod(mc, playermid);
    env->DeleteLocalRef(mc);
    return obj;
}

jobject toadll::c_Minecraft::get_localplayerobjstatic(JNIEnv* env)
{
    auto mcclass = get_mcclass(env);
    auto playermid = get_mid(mcclass, mapping::getPlayer, env);
    auto mc = env->CallStaticObjectMethod(mcclass, get_static_mid(mcclass, mapping::getMinecraft, env));
    auto obj = env->CallObjectMethod(mc, playermid);
    env->DeleteLocalRef(mc);
    env->DeleteLocalRef(mcclass);
    return obj;
}

jobject toadll::c_Minecraft::get_world() 
{
    auto mc = this->get_mc();
	auto worldmid = get_mid(mcclass, mapping::getWorld, env);
	auto obj = !worldmid ? nullptr : env->CallObjectMethod(mc, worldmid);

    env->DeleteLocalRef(mc);

    return obj;
}

jobject toadll::c_Minecraft::get_gamesettings()
{
    auto mc = get_mc();
    auto obj = env->CallObjectMethod(mc, get_mid(mc, mapping::getGameSettings, env));
    env->DeleteLocalRef(mc);
    return obj;
}

bool toadll::c_Minecraft::isInGui()
{
    auto mc = get_mc();
    auto res = env->GetObjectField(mc, get_fid(mc, mappingFields::currentScreenField, env)) != nullptr;
    env->DeleteLocalRef(mc);
    return res;
}

float toadll::c_Minecraft::get_partialTick() 
{
    auto mc = get_mc();
    auto obj = env->CallObjectMethod(mc, get_mid(mc, mapping::getTimer, env));
    env->DeleteLocalRef(mc);
    auto res = env->CallFloatMethod(obj, get_mid(obj, mapping::partialTick, env));
    env->DeleteLocalRef(obj);
    return res;
}

float toadll::c_Minecraft::get_renderPartialTick() 
{
    auto mc = get_mc();
    auto obj = env->CallObjectMethod(mc, get_mid(mc, mapping::getTimer, env));
    env->DeleteLocalRef(mc);
    auto res = env->GetFloatField(obj, get_fid(obj, mappingFields::renderPartialTickField, env));
    env->DeleteLocalRef(obj);
    return res;
}

float toadll::c_Minecraft::get_fov()
{
    auto obj = get_gamesettings();
    auto objklass = env->GetObjectClass(obj);

    auto res = env->GetFloatField(obj, get_fid(objklass, mappingFields::fovField, env));

    env->DeleteLocalRef(obj);
    env->DeleteLocalRef(objklass);

    return res;
}

jobject toadll::c_Minecraft::get_mouseOverObj() 
{
    auto mc = get_mc();
    auto obj = env->CallObjectMethod(mc, get_mid(mc, mapping::getObjectMouseOver, env));
    env->DeleteLocalRef(mc);
    return obj;
}

std::string toadll::c_Minecraft::get_mouseOverStr()
{
    auto obj = get_mouseOverObj();
    if (obj == nullptr)
        return "TYPE=null,";
    auto jstr = static_cast<jstring>(env->CallObjectMethod(obj, get_mid(obj, mapping::toString, env)));

    auto res = jstring2string(jstr, env);
    env->DeleteLocalRef(obj);
    env->DeleteLocalRef(jstr);
    return res;
}

std::shared_ptr<toadll::c_Entity> toadll::c_Minecraft::get_mouseOverPlayer()
{
    if (get_mouseOverStr().find("MISS") != std::string::npos) return nullptr;

    auto obj = get_mouseOverObj();
    if (obj == nullptr)
        return nullptr;

    // TODO: ...
    if (static bool init_map = false; !init_map)
    {
        auto klass = env->GetObjectClass(obj);
        mappings::methodnames.insert({ mapping::getEntityHit, "bridge$getEntityHit" });
        if (!mappings::getsig(mapping::getEntityHit, "bridge$getEntityHit", klass, env))
            log_Error("can't find getEntityHit");
        env->DeleteLocalRef(klass);
        init_map = true;
    }

    auto entityObj = env->CallObjectMethod(obj, get_mid(obj, mapping::getEntityHit, env));
    env->DeleteLocalRef(obj);
    if (entityObj == nullptr)
        return nullptr;
    return std::make_shared<c_Entity>(entityObj, env, get_entity_living_class());
}

bool toadll::c_Minecraft::is_AirBlock(jobject blockobj)
{
    auto world = get_world();
    auto res = env->CallBooleanMethod(world, get_mid(world, mapping::isAirBlock, env), blockobj);
    env->DeleteLocalRef(world);
    return res;
}

std::vector<std::shared_ptr<toadll::c_Entity>> toadll::c_Minecraft::get_playerList()
{
    auto world = get_world();
    if (!world) 
        return {};

    //auto entities = env->CallObjectMethod(world, get_mid(world, mapping::getPlayerEntities, env
    auto entities = env->GetObjectField(world, get_fid(world, mappingFields::playerEntitiesField, env));
    if (env->ExceptionCheck())
    {
#ifdef ENABLE_LOGGING
    	log_Error("Exception getting player entities");
        env->ExceptionDescribe();
        SLOW_SLEEP(1000);
#endif
        return {};
    }
    env->DeleteLocalRef(world);

    if (!entities)
        return {};

    auto entitesklass = env->GetObjectClass(entities);
   
    auto to_arraymid = env->GetMethodID(entitesklass, "toArray", "()[Ljava/lang/Object;");
   
    env->DeleteLocalRef(entitesklass);

    auto entityarray = (jobjectArray)env->CallObjectMethod(entities, to_arraymid);
   
    env->DeleteLocalRef(entities);
    if (!entityarray)
        return {};

    int size = env->GetArrayLength(entityarray);
   
    std::vector<std::shared_ptr<c_Entity>> res = {};
    res.reserve(size);

    for (int i = 0; i < size; i++)
    {
        jobject obj = env->GetObjectArrayElement(entityarray, i);
        res.emplace_back(std::make_shared<c_Entity>(obj, env, get_entity_living_class()));
    }

    env->DeleteLocalRef(entityarray);

    return res;
}

