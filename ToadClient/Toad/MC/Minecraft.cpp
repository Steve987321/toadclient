#include "pch.h"
#include "Toad/Toad.h"
#include "Toad/MC/Utils/mcutils.h"
#include "Minecraft.h"

namespace toadll
{

Minecraft::~Minecraft()
{
    if (m_mcclass != nullptr) env->DeleteGlobalRef(m_mcclass);
    if (m_elbclass != nullptr) env->DeleteGlobalRef(m_elbclass);
    if (m_ariclass != nullptr) env->DeleteGlobalRef(m_ariclass);
    if (m_vec3class != nullptr) env->DeleteGlobalRef(m_vec3class);
    if (m_mopclass != nullptr) env->DeleteGlobalRef(m_mopclass);
}

jclass Minecraft::findMcClass(JNIEnv* env)
{
    return findclass(toad::g_curr_client == toad::MC_CLIENT::Vanilla ? "ave" : "net.minecraft.client.Minecraft", env);
}

jclass Minecraft::getMcClass()
{
    if (m_mcclass == nullptr)
    {
        m_mcclass = findclass(toad::g_curr_client == toad::MC_CLIENT::Vanilla ? "ave" : "net.minecraft.client.Minecraft", env);
    }
    return m_mcclass;
}

jclass Minecraft::getEntityLivingClass()
{
    if (m_elbclass == nullptr)
    {
        m_elbclass = (jclass)env->NewGlobalRef(findclass("net.minecraft.entity.EntityLivingBase", env));
    }
    return m_elbclass;
}

jclass Minecraft::getVec3Class()
{
    if (m_vec3class == nullptr)
    {
        m_vec3class = (jclass)env->NewGlobalRef(findclass("net.minecraft.util.Vec3", env));
    }
    return m_vec3class;
}

jclass Minecraft::getVec3iClass()
{
    if (m_vec3iclass == nullptr)
    {
        m_vec3iclass = (jclass)env->NewGlobalRef(findclass("net.minecraft.util.Vec3i", env));
    }
    return m_vec3iclass;
}

jclass Minecraft::getMovingObjPosClass()
{
    if (m_mopclass == nullptr)
    {
        m_mopclass = (jclass)env->NewGlobalRef(findclass("net.minecraft.util.MovingObjectPosition", env));
    }
    return m_mopclass;
}

std::unique_ptr<ActiveRenderInfo> Minecraft::getActiveRenderInfo()
{
    if (m_ariclass == nullptr)
    {
        m_ariclass = (jclass)env->NewGlobalRef(findclass("net.minecraft.client.renderer.ActiveRenderInfo", env));
    }

    return std::make_unique<ActiveRenderInfo>(m_ariclass, env);
}


//std::unique_ptr<RenderManager> Minecraft::getRenderManager()
//{
//    auto mc = getMc();
//    auto mId = get_mid(mc, mapping::getRenderManager, env);
//    auto obj = env->CallObjectMethod(mc, mId);
//    env->DeleteLocalRef(mc);
//
//    return std::make_unique<RenderManager>(env->NewGlobalRef(obj), env); 
//}

jobject Minecraft::getMc()
{
    return env->CallStaticObjectMethod(getMcClass(), get_static_mid(getMcClass(), mapping::getMinecraft, env));
}

std::shared_ptr<c_Entity> Minecraft::getLocalPlayer()
{
    auto mc = getMc();
    auto mId = get_mid(mc, mapping::getPlayer, env);
    if (!mId)
    {
        env->DeleteLocalRef(mc);
        return nullptr;
    }
    auto obj = env->CallObjectMethod(mc, mId);
    if (!obj)
    {
        env->DeleteLocalRef(mc);
        return nullptr;
    }
    /* auto fId = get_fid(mc, mappingFields::thePlayerField, env);
     if (!fId)
         return nullptr;
     auto obj = env->GetObjectField(mc, fId);
     if (!obj)
         return nullptr;*/
    env->DeleteLocalRef(mc);
    return std::make_shared<c_Entity>(obj, env, getEntityLivingClass());
}

std::string Minecraft::getMouseOverTypeStr()
{
    auto str = getMouseOverStr();
    auto start = str.find("type=") + 5;
    if (start == std::string::npos) return "";
    auto end = str.find(',', start);
    return str.substr(start, end - start);
}

int Minecraft::getLeftClickCounter()
{
    auto fId = get_fid(getMcClass(), mappingFields::leftClickCounterField, env);

    if (!fId)
    {
        return -1;
    }

    auto mc = getMc();

    auto res = env->GetIntField(mc, fId);

    env->DeleteLocalRef(mc);

    return res;
}

Minecraft::RAYTRACE_BLOCKS_RESULT Minecraft::rayTraceBlocks(Vec3 from, Vec3 direction, Vec3& result, bool stopOnLiquid, bool stopOnAirBlock, int subtractY)
{
    auto world = getWorld();

    auto mId = get_mid(world, mapping::rayTraceBlocks, env);
    if (!mId)
    {
        env->DeleteLocalRef(world);
        result = { -1.5f, -1.5f, -1.5f };
        return RAYTRACE_BLOCKS_RESULT::ERROR;
    }

    auto vec3InitMId = get_mid(getVec3Class(), mapping::Vec3Init, env);
    if (!vec3InitMId)
    {
        env->DeleteLocalRef(world);
        result = { -1.5f, -1.5f, -1.5f };
        return RAYTRACE_BLOCKS_RESULT::ERROR;
    }

    // create our Vec3 objects
    auto fromObj = env->NewObject(getVec3Class(), vec3InitMId, from.x, from.y, from.z);
    bool originIsAir = false;
    if (stopOnAirBlock)
    {
        auto vec3iInitMId = get_mid(getVec3iClass(), mapping::Vec3IInit, env);
        auto fromObjUnder = env->NewObject(getVec3iClass(), vec3iInitMId, (int)from.x, (int)(from.y - subtractY), (int)from.z);

        if (isAirBlock(fromObjUnder))
            originIsAir = true;
        
        env->DeleteLocalRef(fromObjUnder);
    }
    auto directionObj = env->NewObject(getVec3Class(), vec3InitMId, direction.x, direction.y, direction.z);

    // call the rayTraceBlocks function 
    auto movingObjPosObj = env->CallObjectMethod(world, mId, fromObj, directionObj, stopOnLiquid);

    // we don't need these anymore 
    env->DeleteLocalRef(world);
    env->DeleteLocalRef(fromObj);
    env->DeleteLocalRef(directionObj);

    // check if we didn't hit any thing
    if (movingObjPosObj == nullptr)
    {
        result = { -1.5f, -1.5f, -1.5f };
        return originIsAir ? RAYTRACE_BLOCKS_RESULT::NO_HIT_FROM_AIRBLOCK : RAYTRACE_BLOCKS_RESULT::NO_HIT;
    }

    auto blockPosMId = get_mid(movingObjPosObj, mapping::getBlockPositionFromMovingBlock, env);

    if (!blockPosMId)
    {
        result = { -1.5f,-1.5f,-1.5f };
        return RAYTRACE_BLOCKS_RESULT::ERROR;
    }

    auto blockPosObj = env->CallObjectMethod(movingObjPosObj, blockPosMId);
    env->DeleteLocalRef(movingObjPosObj);

    result = to_vec3i(blockPosObj, env);

    env->DeleteLocalRef(blockPosObj);
    return originIsAir ? RAYTRACE_BLOCKS_RESULT::HIT_FROM_AIRBLOCK : RAYTRACE_BLOCKS_RESULT::HIT;

}

std::string Minecraft::movingObjPosToStr(jobject mopObj)
{
    auto mId = get_mid(mopObj, mapping::toString, env);
	if (!mId)
	{
        return "";
	}

    auto jstr = static_cast<jstring>(env->CallObjectMethod(mopObj, mId));
    if (!jstr)
        return "";

    auto res = jstring2string(jstr, env);

    env->DeleteLocalRef(jstr);
    return res;
}

void Minecraft::set_gamma(float val)
{
    auto obj = getGameSettings();

    auto mId = get_mid(obj, mapping::setGamma, env);
    if (!mId)
        return;

    env->CallVoidMethod(obj, mId, val);

    env->DeleteLocalRef(obj);
}

void Minecraft::setObjMouseOver(jobject newMopObj)
{
    auto mc = getMc();
    auto fid = get_fid(getMcClass(), mappingFields::objMouseOver, env);
    if (fid != nullptr)
    	env->SetObjectField(mc, fid, newMopObj);
    env->DeleteLocalRef(mc);
}

void Minecraft::setLeftClickCounter(int val)
{
    auto fId = get_fid(getMcClass(), mappingFields::leftClickCounterField, env);

    if (!fId)
    {
        return;
    }

    auto mc = getMc();

    env->SetIntField(mc, fId, val);
}

//void c_Minecraft::disableLightMap() const
//{
//    auto EntityRenderer = env->CallObjectMethod(get_mc(), get_mid(mcclass, mapping::getEntityRenderer));
//    env->CallVoidMethod(EntityRenderer, get_mid(EntityRenderer, mapping::disableLightmap));
//    env->DeleteLocalRef(EntityRenderer);
//}
//
//void c_Minecraft::enableLightMap() const
//{
//    auto EntityRenderer = env->CallObjectMethod(get_mc(), get_mid(mcclass, mapping::getEntityRenderer));
//    // void 
//    env->CallVoidMethod(EntityRenderer, get_mid(EntityRenderer, mapping::enableLightmap));
//    env->DeleteLocalRef(EntityRenderer);
//}

jobject Minecraft::getLocalPlayerObject()
{
    auto playermid = get_mid(getMcClass(), mapping::getPlayer, env);
    if (!playermid)
        return nullptr;
    auto mc = this->getMc();
    auto obj = env->CallObjectMethod(mc, playermid);
    env->DeleteLocalRef(mc);
    return obj;
}

jobject Minecraft::getWorld()
{
    auto mc = this->getMc();
    if (!mc)
        return nullptr;
    auto worldFid = get_fid(getMcClass(), mappingFields::theWorldField, env);
    auto obj = !worldFid ? nullptr : env->GetObjectField(mc, worldFid);
    //auto worldmid = get_mid(mcclass, mapping::getWorld, env);
    //auto obj = !worldmid ? nullptr : env->CallObjectMethod(mc, worldmid);

    env->DeleteLocalRef(mc);

    return obj;
}

jobject Minecraft::getGameSettings()
{
    auto mc = getMc();
    auto mId = get_mid(mc, mapping::getGameSettings, env);
    if (!mId)
    {
        env->DeleteLocalRef(mc);
        return nullptr;
    }
    auto obj = env->CallObjectMethod(mc, mId);
    env->DeleteLocalRef(mc);
    return obj;
}

bool Minecraft::isInGui()
{
    if (toad::g_is_ui_internal && CInternalUI::MenuIsOpen)
        return true;

    auto mc = getMc();
    auto fId = get_fid(mc, mappingFields::currentScreenField, env);
    if (!fId)
    {
        env->DeleteLocalRef(mc);
        return false;
    }
    auto res = env->GetObjectField(mc, fId) != nullptr;
    env->DeleteLocalRef(mc);
    return res;
}

float Minecraft::getPartialTick()
{
    auto mc = getMc();
    auto mId = get_mid(mc, mapping::getTimer, env);
    if (!mId)
        return 1;
    auto obj = env->CallObjectMethod(mc, mId);
    env->DeleteLocalRef(mc);
    if (!obj)
        return 1;
    auto res = env->CallFloatMethod(obj, get_mid(obj, mapping::partialTick, env));
    env->DeleteLocalRef(obj);
    return res;
}

float Minecraft::getRenderPartialTick()
{
    auto mc = getMc();
    auto mId = get_mid(mc, mapping::getTimer, env);
    if (!mId)
    {
        env->DeleteLocalRef(mc);
        return 1;
    }
    auto obj = env->CallObjectMethod(mc, mId);
    env->DeleteLocalRef(mc);
    if (!obj)
    {
        env->DeleteLocalRef(mc);
        return 1;
    }

    auto fId = get_fid(obj, mappingFields::renderPartialTickField, env);
    if (!fId)
    {
        env->DeleteLocalRef(obj);
        env->DeleteLocalRef(mc);
        return 1;
    }

    auto res = env->GetFloatField(obj, fId);
    env->DeleteLocalRef(obj);
    env->DeleteLocalRef(mc);
    return res;
}

float Minecraft::getFov()
{
    auto obj = getGameSettings();
    auto fId = get_fid(obj, mappingFields::fovField, env);
    if (!fId)
        return 0;

    auto res = env->GetFloatField(obj, fId);
    env->DeleteLocalRef(obj);
    return res;
}

jobject Minecraft::getMouseOverObject()
{
    auto mc = getMc();
    /*auto mId = get_mid(mc, mapping::getObjectMouseOver, env);
    if (!mId)
    {
        env->DeleteLocalRef(mc);
        return nullptr;
    }*/
    auto fid = get_fid(getMcClass(), mappingFields::objMouseOver, env);
    jobject res = nullptr;
    if (fid != nullptr)
        res = env->GetObjectField(mc, fid);
    //auto obj = env->CallObjectMethod(mc, mId);
    env->DeleteLocalRef(mc);
    return res;
}

int Minecraft::getBlockIdAt(const Vec3& pos)
{
    auto world = getWorld();
    if (!world)
        return 0;
    auto mId = get_mid(world, mapping::getBlockAt, env);
    if (!mId)
    {
        env->DeleteLocalRef(world);
        return 0;
    }

    auto blockatObj = env->CallObjectMethod(world, mId, pos.x, pos.y, pos.z);
    auto blockatkClass = env->GetObjectClass(blockatObj);
    auto id = env->CallStaticIntMethod(blockatkClass, get_static_mid(blockatkClass, mapping::getIdFromBlockStatic, env), blockatObj);

    env->DeleteLocalRef(world);
    env->DeleteLocalRef(blockatObj);
    env->DeleteLocalRef(blockatkClass);

    return id;
}

std::string Minecraft::getMouseOverStr()
{
    auto obj = getMouseOverObject();
    if (obj == nullptr)
        return "TYPE=null,";
    auto mId = get_mid(obj, mapping::toString, env);
    if (!mId)
    {
        env->DeleteLocalRef(obj);
        return "TYPE=null,";
    }

    auto jstr = static_cast<jstring>(env->CallObjectMethod(obj, mId));

    auto res = jstring2string(jstr, env);
    env->DeleteLocalRef(obj);
    env->DeleteLocalRef(jstr);
    return res;
}

std::shared_ptr<c_Entity> Minecraft::getMouseOverPlayer()
{
    auto str = getMouseOverStr();
    if (str.find("ENTITY") == std::string::npos) return nullptr;

    auto obj = getMouseOverObject();
    // TODO: ...
    if (static bool init_map = false; !init_map)
    {
        auto klass = env->GetObjectClass(obj);
        if (!klass)
        {
            env->DeleteLocalRef(obj);
            return nullptr;
        }
        mappings::methodnames.insert({ mapping::getEntityHit, "bridge$getEntityHit" });
        if (!mappings::getsig(mapping::getEntityHit, "bridge$getEntityHit", klass, env))
            LOGERROR("can't find getEntityHit");
        env->DeleteLocalRef(klass);
        init_map = true;
    }

    auto mId = get_mid(obj, mapping::getEntityHit, env);
    if (!mId)
    {
        env->DeleteLocalRef(obj);
        return nullptr;
    }

    auto entityObj = env->CallObjectMethod(obj, mId);
    env->DeleteLocalRef(obj);
    if (entityObj == nullptr)
        return nullptr;

    return std::make_shared<c_Entity>(entityObj, env, getEntityLivingClass());
}

bool Minecraft::isAirBlock(jobject blockobj)
{
    auto world = getWorld();
    auto mId = get_mid(world, mapping::isAirBlock, env);
    if (!mId)
        return false;
    auto res = env->CallBooleanMethod(world, mId, blockobj);
    env->DeleteLocalRef(world);
    return res;
}

std::vector<std::shared_ptr<c_Entity>> Minecraft::getPlayerList()
{
    auto world = getWorld();
    if (!world)
        return {};
    
     auto fId = get_fid(world, mappingFields::playerEntitiesField, env);
     if (!fId)
     {
         env->DeleteLocalRef(world);
         return {};
     }

    auto entities = env->GetObjectField(world, fId);

    env->DeleteLocalRef(world);

    if (!entities)
        return {};

    auto entitesklass = env->GetObjectClass(entities);
    if (!entitesklass)
    {
        env->DeleteLocalRef(entities);
        return {};
    }

    static auto to_arraymid = env->GetMethodID(entitesklass, "toArray", "()[Ljava/lang/Object;");
    env->DeleteLocalRef(entitesklass);

    auto entityarray = (jobjectArray)env->CallObjectMethod(entities, to_arraymid);
    env->DeleteLocalRef(entities);

    if (!entityarray)
        return {};

    int size = env->GetArrayLength(entityarray);
    if (size == 0)
    {
        env->DeleteLocalRef(entityarray);
        return {};
    }

    std::vector<std::shared_ptr<c_Entity>> res = {};
    res.reserve(size);

    for (int i = 0; i < size; i++)
    {
        jobject obj = env->GetObjectArrayElement(entityarray, i);

        if (!obj)
            continue;

        res.emplace_back(std::make_shared<c_Entity>(obj, env, getEntityLivingClass()));
    }

    env->DeleteLocalRef(entityarray);

    return res;
}

}