#include "pch.h"
#include "Toad/toadll.h"
#include "Toad/MC/mcutils.h"
#include "minecraft.h"

namespace toadll
{

Minecraft::~Minecraft()
{
    if (m_mcclass != nullptr) env->DeleteGlobalRef(m_mcclass);
    if (m_elbclass != nullptr) env->DeleteGlobalRef(m_elbclass);
    if (m_ariclass != nullptr) env->DeleteGlobalRef(m_ariclass);
    if (m_vec3class != nullptr) env->DeleteGlobalRef(m_vec3class);
    if (m_mopclass != nullptr) env->DeleteGlobalRef(m_mopclass);
    if (m_blockposclass != nullptr) env->DeleteGlobalRef(m_blockposclass);
}

jclass Minecraft::getMcClass(JNIEnv* env)
{
    return findclass(toad::g_curr_client == toad::MC_CLIENT::NOT_SUPPORTED ? unsupported_mc_class_name.c_str() : "net.minecraft.client.Minecraft", env);
}

jclass Minecraft::getMcClass()
{
    if (m_mcclass == nullptr)
    {
        m_mcclass = findclass(toad::g_curr_client == toad::MC_CLIENT::NOT_SUPPORTED ? "ave" : "net.minecraft.client.Minecraft", env);
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

jclass Minecraft::getBlockPosClass()
{
	if (m_blockposclass == nullptr)
    {
        m_blockposclass = (jclass)env->NewGlobalRef(findclass("net.minecraft.util.BlockPos", env));
    }
    return m_blockposclass;
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
    auto fid = get_static_fid(getMcClass(), mappingFields::theMcField, env);

    if (!fid)
    {
        // this should not happen so also close
        g_is_running = false;
        return nullptr;
    }

    return env->GetStaticObjectField(getMcClass(), fid);
}

std::shared_ptr<c_Entity> Minecraft::getLocalPlayer()
{
    auto mc = getMc();
    auto fid = get_fid(mc, mappingFields::thePlayerField, env);

    if (!fid)
    {
        env->DeleteLocalRef(mc);
        return nullptr;
    }
    auto obj = env->GetObjectField(mc, fid);
    if (!obj)
    {
        env->DeleteLocalRef(mc);
        return nullptr;
    }
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

//void Minecraft::set_gamma(float val)
//{
//    auto obj = getGameSettings();
//    auto mId = get_mid(obj, mapping::setGamma, env);
//    if (!mId)
//        return;
//
//    env->CallVoidMethod(obj, mId, val);
//
//    env->DeleteLocalRef(obj);
//}

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

jobject Minecraft::getWorld()
{
    auto mc = getMc();
    if (!mc)
        return nullptr;
    auto worldFid = get_fid(getMcClass(), mappingFields::theWorldField, env);
    auto obj = !worldFid ? nullptr : env->GetObjectField(mc, worldFid);
    env->DeleteLocalRef(mc);

    return obj;
}

jobject Minecraft::getGameSettings()
{
    auto mc = getMc();
    auto fid = get_fid(mc, mappingFields::gameSettings, env);
    if (!fid)
    {
        env->DeleteLocalRef(mc);
        return nullptr;
    }
    auto obj = env->GetObjectField(mc, fid);

    if (static bool once = true; once)
    {
        LOGDEBUG("GAMESETTINGS");
        jclass klass = env->GetObjectClass(obj);
        loop_through_class(klass, env);
        env->DeleteLocalRef(klass);
        once = false;
    }

    env->DeleteLocalRef(mc);
    return obj;
}

bool Minecraft::isInGui()
{
    if (toad::g_is_ui_internal && CInternalUI::MenuIsOpen)
        return true;

    jobject mc = getMc();
    jfieldID fId = get_fid(mc, mappingFields::currentScreenField, env);
    if (!fId)
    {
        env->DeleteLocalRef(mc);
        return false;
    }
    jobject obj = env->GetObjectField(mc, fId);

	env->DeleteLocalRef(obj);
	env->DeleteLocalRef(mc);

    return obj != nullptr;
}

std::array<std::string, 27> Minecraft::GetChestContents()
{
    std::array<std::string, 27> res;

	jobject mc = getMc();
	jfieldID current_screen_fid = get_fid(mc, mappingFields::currentScreenField, env);

	if (!current_screen_fid)
	{
		env->DeleteLocalRef(mc);
		return res;
	}

	jobject current_screen_obj = env->GetObjectField(mc, current_screen_fid);

    if (!current_screen_obj)
    {
        env->DeleteLocalRef(mc);
        return res;
	}

	jclass current_screen_class = env->GetObjectClass(current_screen_obj);
	jfieldID lower_chest_fid = get_fid(current_screen_class, mappingFields::lowerChestInventory, env);

    if (lower_chest_fid)
    {
        jobject invobj = env->GetObjectField(current_screen_obj, lower_chest_fid);
        jclass invobjklass = env->GetObjectClass(invobj);
		jmethodID get_stack_in_slot = get_mid(invobjklass, mapping::getStackInSlot, env);

        if (!get_stack_in_slot)
        {
            env->DeleteLocalRef(invobj);
            env->DeleteLocalRef(invobjklass);
            env->DeleteLocalRef(current_screen_class);
            env->DeleteLocalRef(mc);
            return res;
        }

        for (int i = 0; i < 27; i++)
        {
            jobject item_stack_obj = env->CallObjectMethod(invobj, get_stack_in_slot, i);

            if (item_stack_obj)
            {
                jstring item_str_obj = (jstring)env->CallObjectMethod(item_stack_obj, get_mid(item_stack_obj, mapping::getDisplayName, env));
                res[i] = jstring2string(item_str_obj, env);
                env->DeleteLocalRef(item_str_obj);
            }

            env->DeleteLocalRef(item_stack_obj);
        }

        env->DeleteLocalRef(invobj);
        env->DeleteLocalRef(invobjklass);
    }

    env->DeleteLocalRef(current_screen_class);
	env->DeleteLocalRef(current_screen_obj);

    env->DeleteLocalRef(mc);
    return res;
}

float Minecraft::getPartialTick()
{
    jobject mc = getMc();
    jfieldID fid = get_fid(mc, mappingFields::timer, env);
    if (!fid)
        return 1;
    jobject obj = env->GetObjectField(mc, fid);
    env->DeleteLocalRef(mc);
    if (!obj)
        return 1;

    jfieldID partialtick_fid = get_fid(obj, mappingFields::elapsedPartialTicks, env);
    if (!partialtick_fid)
		return 1;

    float res = env->GetFloatField(obj, partialtick_fid);
    env->DeleteLocalRef(obj);
    return res;
}

float Minecraft::getRenderPartialTick()
{
    auto mc = getMc();
    auto fid = get_fid(mc, mappingFields::timer, env);
    if (!fid)
    {
        env->DeleteLocalRef(mc);
        return 1;
    }
    auto obj = env->GetObjectField(mc, fid);
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

//float Minecraft::getFov()
//{
//    auto obj = getGameSettings();
//    auto fId = get_fid(obj, mappingFields::fovField, env);
//    if (!fId)
//        return 0;
//
//    auto res = env->GetFloatField(obj, fId);
//    env->DeleteLocalRef(obj);
//    return res;
//}

jobject Minecraft::getMouseOverObject()
{
    jobject mc = getMc();
    jfieldID fid = get_fid(getMcClass(), mappingFields::objMouseOver, env);
    jobject res = nullptr;

    if (fid != nullptr)
        res = env->GetObjectField(mc, fid);

    env->DeleteLocalRef(mc);
    return res;
}

int Minecraft::getBlockIdAt(const Vec3i& pos)
{
    jobject world = getWorld();
    if (!world)
        return 0;
    
    jclass block_pos_class = getBlockPosClass();
    jmethodID block_pos_mid = get_mid(block_pos_class, mapping::Vec3IInit, env);
    if (!block_pos_mid)
    {
        env->DeleteLocalRef(world);
		return 0;
    }
    jobject block_pos = env->NewObject(block_pos_class, block_pos_mid, pos.x, pos.y, pos.z);
    if (!block_pos)
    {
		env->DeleteLocalRef(world);
        return 0;
    }

    jmethodID get_block_state_mid = get_mid(world, mapping::getBlockState, env);
    if (!get_block_state_mid)
    {
		env->DeleteLocalRef(block_pos);
		env->DeleteLocalRef(world);
        return 0;
    }
    jobject block_state = env->CallObjectMethod(world, get_block_state_mid, block_pos);

	env->DeleteLocalRef(world);
	env->DeleteLocalRef(block_pos);

    jclass block_state_class = env->GetObjectClass(block_state);

    jmethodID get_block_mid = get_mid(block_state_class, mapping::getBlock, env);
    jobject block = env->CallObjectMethod(block_state, get_block_mid);
    jclass block_class = env->GetObjectClass(block);
    
    jmethodID get_id_from_blockstate_mid = get_static_mid(block_class, mapping::getIdFromBlockStatic, env);
    
    int id = env->CallStaticIntMethod(block_class, get_id_from_blockstate_mid, block);
    
    env->DeleteLocalRef(block);
    env->DeleteLocalRef(block_class);
    env->DeleteLocalRef(block_state);
    env->DeleteLocalRef(block_state_class);

    return id;
}

std::string Minecraft::getMouseOverStr()
{
    jobject obj = getMouseOverObject();
    if (obj == nullptr)
        return "TYPE=null,";
    jmethodID mid = get_mid(obj, mapping::toString, env);
    if (!mid)
    {
        env->DeleteLocalRef(obj);
        return "TYPE=null,";
    }

    jstring jstr = static_cast<jstring>(env->CallObjectMethod(obj, mid));

    std::string res = jstring2string(jstr, env);
    env->DeleteLocalRef(obj);
    env->DeleteLocalRef(jstr);
    return res;
}

std::shared_ptr<c_Entity> Minecraft::getMouseOverPlayer()
{
	jobject obj = getMouseOverObject();

    jfieldID fid = get_fid(obj, mappingFields::entityHit, env);
    if (!fid)
    {
        env->DeleteLocalRef(obj);
        return nullptr;
    }

    jobject entity = env->GetObjectField(obj, fid);
    env->DeleteLocalRef(obj);

    if (entity == nullptr)
        return nullptr;

    return std::make_shared<c_Entity>(entity, env, getEntityLivingClass());
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