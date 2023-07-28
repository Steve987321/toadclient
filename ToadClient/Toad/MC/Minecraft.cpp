#include "pch.h"
#include "Toad/Toad.h"
#include "Toad/MC/Utils/mcutils.h"
#include "Minecraft.h"

toadll::Minecraft::~Minecraft()
{
    env->DeleteLocalRef(mcclass);
    if (m_elbclass != nullptr) env->DeleteGlobalRef(m_elbclass);
    if (m_ariclass != nullptr) env->DeleteGlobalRef(m_ariclass);
    if (m_vec3class != nullptr) env->DeleteGlobalRef(m_vec3class);
}

jclass toadll::Minecraft::getMcClass(JNIEnv* env)
{
    return findclass(toad::g_curr_client == toad::MC_CLIENT::Vanilla ? "ave" : "net.minecraft.client.Minecraft", env);
}

jclass toadll::Minecraft::getEntityLivingClass()
{
	if (m_elbclass == nullptr)
	{
        m_elbclass = (jclass)env->NewGlobalRef(findclass("net.minecraft.entity.EntityLivingBase", env));
	}
    return m_elbclass;
}

jclass toadll::Minecraft::getVec3Class()
{
    if (m_vec3class == nullptr)
    {
        m_vec3class = (jclass)env->NewGlobalRef(findclass("net.minecraft.util.Vec3", env));
    }
    return m_vec3class;
}

std::unique_ptr<toadll::ActiveRenderInfo> toadll::Minecraft::getActiveRenderInfo()
{
	if (m_ariclass == nullptr)
	{
        m_ariclass = (jclass)env->NewGlobalRef(findclass("net.minecraft.client.renderer.ActiveRenderInfo", env));
	}
    
    return std::make_unique<ActiveRenderInfo>(m_ariclass, env);
}

 
jobject toadll::Minecraft::getMc() const
{
    return env->CallStaticObjectMethod(mcclass, get_static_mid(mcclass, mapping::getMinecraft, env));
}

jobject toadll::Minecraft::getRenderManager()
{
    auto mc = getMc();
    auto mId = get_mid(mc, mapping::getRenderManager, env);
    if (!mId)
        return nullptr;
    auto ret = env->CallObjectMethod(mc, mId);
    env->DeleteLocalRef(mc); 
    return ret;
}

std::shared_ptr<toadll::c_Entity> toadll::Minecraft::getLocalPlayer()
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

toadll::Vec3 toadll::Minecraft::rayTraceBlocks(Vec3 from, Vec3 direction, bool stopOnLiquid)
{
    auto world = getWorld();

    auto mId = get_mid(world, mapping::rayTraceBlocks, env);
    if (!mId)
    {
        env->DeleteLocalRef(world);
        return { -1.5f, -1.5f, -1.5f };
    }

    auto vec3InitMId = get_mid(getVec3Class(), mapping::Vec3Init, env);
    if (!vec3InitMId)
    {
        env->DeleteLocalRef(world);
        return { -1.5f, -1.5f, -1.5f };
    }

    // create our Vec3 objects
    auto fromObj = env->NewObject(getVec3Class(), vec3InitMId, from.x, from.y, from.z);
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
        return { -0.5f, -0.5f, -0.5f };
    }

    auto blockPosMId = get_mid(movingObjPosObj, mapping::getBlockPositionFromMovingBlock, env);

    if (!blockPosMId)
    {
        return { -1.5f,-1.5f,-1.5f };
    }

    auto blockPosObj = env->CallObjectMethod(movingObjPosObj, blockPosMId); 
    env->DeleteLocalRef(movingObjPosObj);

    auto res = to_vec3i(blockPosObj, env);

    env->DeleteLocalRef(blockPosObj);
    return res;

}

void toadll::Minecraft::set_gamma(float val)
{
    auto obj = getGameSettings();

    auto mId = get_mid(obj, mapping::setGamma, env);
    if (!mId)
        return;

    env->CallVoidMethod(obj, mId, val);

    env->DeleteLocalRef(obj);
}

//void toadll::c_Minecraft::disableLightMap() const
//{
//    auto EntityRenderer = env->CallObjectMethod(get_mc(), get_mid(mcclass, mapping::getEntityRenderer));
//    env->CallVoidMethod(EntityRenderer, get_mid(EntityRenderer, mapping::disableLightmap));
//    env->DeleteLocalRef(EntityRenderer);
//}
//
//void toadll::c_Minecraft::enableLightMap() const
//{
//    auto EntityRenderer = env->CallObjectMethod(get_mc(), get_mid(mcclass, mapping::getEntityRenderer));
//    // void 
//    env->CallVoidMethod(EntityRenderer, get_mid(EntityRenderer, mapping::enableLightmap));
//    env->DeleteLocalRef(EntityRenderer);
//}

jobject toadll::Minecraft::getLocalPlayerObject()
{
    auto playermid = get_mid(mcclass, mapping::getPlayer, env);
    if (!playermid)
        return nullptr;
    auto mc = this->getMc();
    auto obj = env->CallObjectMethod(mc, playermid);
    env->DeleteLocalRef(mc);
    return obj;
}

jobject toadll::Minecraft::getWorld() 
{
    auto mc = this->getMc();
    if (!mc)
        return nullptr;
    auto worldFid = get_fid(mcclass, mappingFields::theWorldField, env);
    auto obj = !worldFid ? nullptr : env->GetObjectField(mc, worldFid);
	//auto worldmid = get_mid(mcclass, mapping::getWorld, env);
	//auto obj = !worldmid ? nullptr : env->CallObjectMethod(mc, worldmid);

    env->DeleteLocalRef(mc);

    return obj;
}

jobject toadll::Minecraft::getGameSettings()
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

bool toadll::Minecraft::isInGui()
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

float toadll::Minecraft::getPartialTick() 
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

float toadll::Minecraft::getRenderPartialTick() 
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

float toadll::Minecraft::getFov()
{
    auto obj = getGameSettings();
    auto fId = get_fid(obj, mappingFields::fovField, env);
    if (!fId)
        return 0;

    auto res = env->GetFloatField(obj, fId);
    env->DeleteLocalRef(obj);
    return res;
}

jobject toadll::Minecraft::getMouseOverBlock() 
{
    auto mc = getMc();
    auto mId = get_mid(mc, mapping::getObjectMouseOver, env);
    if (!mId)
    {
        env->DeleteLocalRef(mc);
        return nullptr;
    }
    auto obj = env->CallObjectMethod(mc, mId);
    env->DeleteLocalRef(mc);
    return obj;
}

int toadll::Minecraft::getBlockIdAt(const Vec3& pos)
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

std::string toadll::Minecraft::getMouseOverBlockStr()
{
    auto obj = getMouseOverBlock();
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

std::shared_ptr<toadll::c_Entity> toadll::Minecraft::getMouseOverPlayer()
{
    auto str = getMouseOverBlockStr();
    if (str.find("MISS") != std::string::npos || str.find("null") != std::string::npos) return nullptr;

    auto obj = getMouseOverBlock();
    if (obj == nullptr)
        return nullptr;

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

bool toadll::Minecraft::isAirBlock(jobject blockobj)
{
    auto world = getWorld();
    auto mId = get_mid(world, mapping::isAirBlock, env);
    if (!mId)
        return false;
    auto res = env->CallBooleanMethod(world, mId, blockobj);
    env->DeleteLocalRef(world);
    return res;
}

std::vector<std::shared_ptr<toadll::c_Entity>> toadll::Minecraft::getPlayerList()
{
    auto world = getWorld();
    if (!world)
        return {};


   /* auto fId = get_fid(world, mappingFields::playerEntitiesField, env);
    if (!fId)
    {
        env->DeleteLocalRef(world);
        return {};
    }*/

    //auto entities = env->GetObjectField(world, fId);
    auto mId = get_mid(world, mapping::getPlayerEntities, env);
 
    auto entities = env->CallObjectMethod(world, mId);

    env->DeleteLocalRef(world);

    if (!entities)
        return {};

    auto entitesklass = env->GetObjectClass(entities);
    if (!entitesklass)
    {
        env->DeleteLocalRef(entities);
        return {};
    }
   
    auto to_arraymid = env->GetMethodID(entitesklass, "toArray", "()[Ljava/lang/Object;");
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
