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

jclass toadll::c_Minecraft::getMcClass(JNIEnv* env)
{
    return findclass(curr_client == minecraft_client::Vanilla ? "ave" : "net.minecraft.client.Minecraft", env);
}

jclass toadll::c_Minecraft::getEntityLivingClass()
{
	if (elbclass == nullptr)
	{
        elbclass = (jclass)env->NewGlobalRef(findclass("net.minecraft.entity.EntityLivingBase", env));
	}
    return elbclass;
}

std::unique_ptr<toadll::c_ActiveRenderInfo> toadll::c_Minecraft::getActiveRenderInfo()
{
	if (ariclass == nullptr)
	{
        ariclass = (jclass)env->NewGlobalRef(findclass("net.minecraft.client.renderer.ActiveRenderInfo", env));
	}
    
    return std::make_unique<c_ActiveRenderInfo>(ariclass, env);
}

 
jobject toadll::c_Minecraft::getMc()
{
    return env->CallStaticObjectMethod(mcclass, get_static_mid(mcclass, mapping::getMinecraft, env));
}

jobject toadll::c_Minecraft::getRenderManager()
{
    auto mc = getMc();
    auto ret = env->CallObjectMethod(mc, get_mid(mc, mapping::getRenderManager, env));
    env->DeleteLocalRef(mc); 
    return ret;
}

std::shared_ptr<toadll::c_Entity> toadll::c_Minecraft::getLocalPlayer()
{
    auto mc = getMc();
    auto obj = env->GetObjectField(mc, get_fid(mc, mappingFields::thePlayerField, env));
    //auto playermid = get_mid(mc, mapping::getPlayer, env);
    //auto obj = !playermid ? nullptr : std::make_shared<c_Entity>(env->CallObjectMethod(mc, playermid), env, get_entity_living_class());
    if (env->ExceptionCheck())
    {
        log_Debug("OH NO EXCEPTION!");
        return nullptr;
    }
    env->DeleteLocalRef(mc);
    return std::make_shared<c_Entity>(obj, env, getEntityLivingClass());
}

void toadll::c_Minecraft::set_gamma(float val)
{
    auto obj = getGameSettings();

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

jobject toadll::c_Minecraft::getLocalPlayerObject()
{
    auto playermid = get_mid(mcclass, mapping::getPlayer, env);
    auto mc = this->getMc();
    auto obj = env->CallObjectMethod(mc, playermid);
    env->DeleteLocalRef(mc);
    return obj;
}

jobject toadll::c_Minecraft::getWorld() 
{
    auto mc = this->getMc();
	auto worldmid = get_mid(mcclass, mapping::getWorld, env);
	auto obj = !worldmid ? nullptr : env->CallObjectMethod(mc, worldmid);
    env->DeleteLocalRef(mc);

    return obj;
}

jobject toadll::c_Minecraft::getGameSettings()
{
    auto mc = getMc();
    auto obj = env->CallObjectMethod(mc, get_mid(mc, mapping::getGameSettings, env));
    env->DeleteLocalRef(mc);
    return obj;
}

bool toadll::c_Minecraft::isInGui()
{
    auto mc = getMc();
    auto res = env->GetObjectField(mc, get_fid(mc, mappingFields::currentScreenField, env)) != nullptr;
    env->DeleteLocalRef(mc);
    return res;
}

float toadll::c_Minecraft::getPartialTick() 
{
    auto mc = getMc();
    auto obj = env->CallObjectMethod(mc, get_mid(mc, mapping::getTimer, env));
    env->DeleteLocalRef(mc);
    if (!obj) return 1;
    auto res = env->CallFloatMethod(obj, get_mid(obj, mapping::partialTick, env));
    env->DeleteLocalRef(obj);
    return res;
}

float toadll::c_Minecraft::getRenderPartialTick() 
{
    auto mc = getMc();
    auto obj = env->CallObjectMethod(mc, get_mid(mc, mapping::getTimer, env));
    env->DeleteLocalRef(mc);
    if (!obj) return 1;
    auto res = env->GetFloatField(obj, get_fid(obj, mappingFields::renderPartialTickField, env));
    env->DeleteLocalRef(obj);
    return res;
}

float toadll::c_Minecraft::getFov()
{
    auto obj = getGameSettings();
    auto objklass = env->GetObjectClass(obj);

    auto res = env->GetFloatField(obj, get_fid(objklass, mappingFields::fovField, env));

    env->DeleteLocalRef(obj);
    env->DeleteLocalRef(objklass);

    return res;
}

jobject toadll::c_Minecraft::getMouseOverBlock() 
{
    auto mc = getMc();
    auto obj = env->CallObjectMethod(mc, get_mid(mc, mapping::getObjectMouseOver, env));
    env->DeleteLocalRef(mc);
    return obj;
}

int toadll::c_Minecraft::getBlockIdAt(const vec3& pos)
{
    auto world = getWorld();
    if (!world) return 0;
    auto blockatObj = env->CallObjectMethod(world, get_mid(world, mapping::getBlockAt, env), pos.x, pos.y, pos.z);
    auto blockatkClass = env->GetObjectClass(blockatObj);
    auto id = env->CallStaticIntMethod(blockatkClass, get_static_mid(blockatkClass, mapping::getIdFromBlockStatic, env), blockatObj);

    env->DeleteLocalRef(world);
    env->DeleteLocalRef(blockatObj);
    env->DeleteLocalRef(blockatkClass);

    return id;
}

std::string toadll::c_Minecraft::getMouseOverBlockStr()
{
    auto obj = getMouseOverBlock();
    if (obj == nullptr)
        return "TYPE=null,";
    auto jstr = static_cast<jstring>(env->CallObjectMethod(obj, get_mid(obj, mapping::toString, env)));

    auto res = jstring2string(jstr, env);
    env->DeleteLocalRef(obj);
    env->DeleteLocalRef(jstr);
    return res;
}

std::shared_ptr<toadll::c_Entity> toadll::c_Minecraft::getMouseOverPlayer()
{
    if (getMouseOverBlockStr().find("MISS") != std::string::npos) return nullptr;

    auto obj = getMouseOverBlock();
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
    return std::make_shared<c_Entity>(entityObj, env, getEntityLivingClass());
}

bool toadll::c_Minecraft::isAirBlock(jobject blockobj)
{
    auto world = getWorld();
    auto res = env->CallBooleanMethod(world, get_mid(world, mapping::isAirBlock, env), blockobj);
    env->DeleteLocalRef(world);
    return res;
}

std::vector<std::shared_ptr<toadll::c_Entity>> toadll::c_Minecraft::getPlayerList()
{
    auto world = getWorld();
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
        env->DeleteLocalRef(world);
        return {};
    }

    env->DeleteLocalRef(world);

    if (!entities)
        return {};

    auto entitesklass = env->GetObjectClass(entities);
   
    auto to_arraymid = env->GetMethodID(entitesklass, "toArray", "()[Ljava/lang/Object;");
    env->DeleteLocalRef(entitesklass);

    if (!to_arraymid)
        return {};

    auto entityarray = (jobjectArray)env->CallObjectMethod(entities, to_arraymid);
    env->DeleteLocalRef(entities);

    if (!entityarray)
        return {};

    int size = env->GetArrayLength(entityarray);
    if (size <= 1)
    {
        env->DeleteLocalRef(entityarray);
        return {};
    }
    std::vector<std::shared_ptr<c_Entity>> res = {};
    res.reserve(size);

    for (int i = 0; i < size; i++)
    {
        jobject obj = env->GetObjectArrayElement(entityarray, i);
        res.emplace_back(std::make_shared<c_Entity>(obj, env, getEntityLivingClass()));
    }

    env->DeleteLocalRef(entityarray);

    return res;
}

