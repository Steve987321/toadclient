#include "pch.h"
#include "Toad/Toad.h"
#include "entity.h"

toadll::c_Entity::c_Entity(jobject jobj, JNIEnv* env, jclass elclass)
	: env(env), elclass(elclass), obj(jobj)
{
	//static bool once = false;
	//if (!once)
	//{
	//	auto klass = env->GetObjectClass(obj);
	//	loop_through_class(elclass, env);
	//	env->DeleteLocalRef(klass);
	//	once = true;
	//}
}

toadll::c_Entity::~c_Entity()
{
	env->DeleteLocalRef(obj);
}

toadll::Vec3 toadll::c_Entity::getPosition() const
{
	auto objklass = env->GetObjectClass(obj);
	auto posXId = get_fid(objklass, mappingFields::EntityPosX, env);
	if (!posXId)
		return {-1, -1, -1};

	auto posYId = get_fid(objklass, mappingFields::EntityPosY, env);
	auto posZId = get_fid(objklass, mappingFields::EntityPosZ, env);

	env->DeleteLocalRef(objklass);

	return Vec3
	{
		static_cast<float>(env->GetDoubleField(obj, posXId)),
		static_cast<float>(env->GetDoubleField(obj, posYId)),
		static_cast<float>(env->GetDoubleField(obj, posZId))
	};
}

toadll::Vec3 toadll::c_Entity::getLastTickPosition() const
{
	auto x = get_fid(obj, mappingFields::lastTickPosXField, env);
	if (!x)
		return { 0,0,0 };
	auto y = get_fid(obj, mappingFields::lastTickPosYField, env);
	auto z = get_fid(obj, mappingFields::lastTickPosZField, env);
	return {
		static_cast<float>(env->GetDoubleField(obj, x)),
		static_cast<float>(env->GetDoubleField(obj, y)),
		static_cast<float>(env->GetDoubleField(obj, z)),
	};
}

float toadll::c_Entity::getRotationYaw() const
{
	//return env->CallDoubleMethod(obj, get_mid(obj, mapping::getRotationYaw, env));
	auto fId = get_fid(obj, mappingFields::rotationYawField, env);
	if (!fId)
		return 0;
	return env->GetFloatField(obj, fId);
}

float toadll::c_Entity::getRotationPitch() const
{
	auto fId = get_fid(obj, mappingFields::rotationPitchField, env);
	if (!fId)
		return 0;
	return env->GetFloatField(obj, fId);
}

jobject toadll::c_Entity::getNameObj() const
{
	auto fId = get_mid(obj, mapping::getName, env);
	if (!fId)
		return nullptr;
	auto strobj = env->CallObjectMethod(obj, fId);
	return strobj;
}

std::string toadll::c_Entity::getName() const
{
	auto mId = get_mid(obj, mapping::getName, env);
	if (!mId)
		return "";

	auto strobj = env->CallObjectMethod(obj, mId);
	auto ret = jstring2string((jstring)strobj, env);
	env->DeleteLocalRef(strobj);
	return ret;
}

std::string toadll::c_Entity::getHeldItemStr() const
{
	auto item = getHeldItem();
	if (item == nullptr)
		return "none";

	auto itemklass = env->GetObjectClass(item);

	auto itemjstr = (jstring)env->CallObjectMethod(item, env->GetMethodID(itemklass, "toString", "()Ljava/lang/String;"));

	auto ret = jstring2string(itemjstr, env);

	env->DeleteLocalRef(item);
	env->DeleteLocalRef(itemklass);
	env->DeleteLocalRef(itemjstr);
	return ret;
}

jobject toadll::c_Entity::getHeldItem() const
{
	auto mId = get_mid(obj, mapping::getHeldItem, env);
	if (!mId) 
		return nullptr;
	return env->CallObjectMethod(obj, mId);
}

std::string toadll::c_Entity::getSlotStr(int slot) const
{
	auto fId = get_fid(obj, mappingFields::inventoryField, env);
	if (!fId)
		return "NONE";

	auto inventory = env->GetObjectField(obj, fId);
	auto mId = get_mid(inventory, mapping::getStackInSlot, env);
	if (!mId)
	{
		env->DeleteLocalRef(inventory);
		return "NONE";
	}

	auto item_at_slot = env->CallObjectMethod(inventory, mId, slot);

	if (!item_at_slot) // no item held
	{
		return "NONE";
	}

	if (static bool once = true; once)
	{
		auto klass = env->GetObjectClass(inventory);
		loop_through_class(klass, env);
		env->DeleteLocalRef(klass);
		once = false;
	}

	auto item_to_string_mid = get_mid(inventory, mapping::toString, env);
	jstring str_obj = (jstring)env->CallObjectMethod(inventory, item_to_string_mid);
	std::string res = jstring2string(str_obj, env);

	env->DeleteLocalRef(str_obj);
	env->DeleteLocalRef(item_at_slot);
	env->DeleteLocalRef(inventory);
	return res;
}

int toadll::c_Entity::getHurtTime() const
{
	//auto mId = get_mid(elclass, mapping::getHurtTime, env);
	//if (!mId)
	//	return 0;
	//auto res = env->CallIntMethod(obj, mId);

	auto fId = get_fid(elclass, mappingFields::hurtTimeI, env);
	if (!fId)
		return 0;
	int res = env->GetIntField(obj, fId);

	return res;
}

float toadll::c_Entity::getHealth() const
{
	auto mid = get_mid(obj, mapping::getHealth, env);
	if (!mid)
		return 0;
	return env->CallFloatMethod(obj, mid);
}

float toadll::c_Entity::getMotionX() const
{
	//return env->CallDoubleMethod(obj, get_mid(obj, mapping::getMotionX));
	auto fId = get_fid(obj, mappingFields::motionXField, env);
	if (!fId)
		return 0;
	return (float)env->GetDoubleField(obj, fId);
}

float toadll::c_Entity::getMotionY() const
{
	//return env->CallDoubleMethod(obj, get_mid(obj, mapping::getMotionY));
	auto fId = get_fid(obj, mappingFields::motionYField, env);
	if (!fId)
		return 0;
	return (float)env->GetDoubleField(obj, fId);
}

float toadll::c_Entity::getMotionZ() const
{
	//return env->CallDoubleMethod(obj, get_mid(obj, mapping::getMotionZ));
	auto fId = get_fid(obj, mappingFields::motionZField, env);
	if (!fId)
		return 0;
	return (float)env->GetDoubleField(obj, fId);
}

bool toadll::c_Entity::isInvisible() const
{
	auto mId = get_mid(obj, mapping::isInvisible, env);
	if (!mId)
		return false;
	return env->CallBooleanMethod(obj, mId);
}

bool toadll::c_Entity::isSneaking() const
{
	auto mid = get_mid(obj, mapping::isSneaking, env);
	return env->CallBooleanMethod(obj, mid);
}

toadll::BBox toadll::c_Entity::get_BBox() const
{
	auto bboxobj = env->CallObjectMethod(obj, get_mid(obj, mapping::getBBox, env));

	float minX = (float)env->CallDoubleMethod(bboxobj, get_mid(bboxobj, mapping::bboxMinX, env));
	float minY = (float)env->CallDoubleMethod(bboxobj, get_mid(bboxobj, mapping::bboxMinY, env));
	float minZ = (float)env->CallDoubleMethod(bboxobj, get_mid(bboxobj, mapping::bboxMinZ, env));
	float maxX = (float)env->CallDoubleMethod(bboxobj, get_mid(bboxobj, mapping::bboxMaxX, env));
	float maxY = (float)env->CallDoubleMethod(bboxobj, get_mid(bboxobj, mapping::bboxMaxY, env));
	float maxZ = (float)env->CallDoubleMethod(bboxobj, get_mid(bboxobj, mapping::bboxMaxZ, env));

	env->DeleteLocalRef(bboxobj);

	return { { minX, minY, minZ }, { maxX, maxY, maxZ } };
}

std::array<std::string, 4> toadll::c_Entity::getArmor()
{
	auto mid = get_mid(obj, mapping::getInventory, env);

	auto invarray = (jobjectArray)env->CallObjectMethod(obj, mid);

	if (!invarray)
		return {};

	std::array<std::string, 4> res;

	// always 4 
	for (int i = 0; i < 4; i++)
	{
		static jmethodID toStrMid = nullptr;

		jobject element = env->GetObjectArrayElement(invarray, i);

		if (!element)
			continue;

		static bool once = false;
		if (!once)
		{
			auto klass = env->GetObjectClass(element);
			toStrMid = get_mid(klass, mapping::toString, env);
			env->DeleteLocalRef(klass);
			once = true;
		}

		auto data = (jstring)env->CallObjectMethod(element, toStrMid);
		auto dataStr = jstring2string(data, env);
		// helmet = 3 boots = 0
		res[i] = dataStr;
		env->DeleteLocalRef(data);
		env->DeleteLocalRef(element);
	}

	env->DeleteLocalRef(invarray);
	return res;
}

//jobject toadll::c_Entity::get_open_container() const
//{
//	return env->CallObjectMethod(obj, get_mid(obj, mapping::getOpenContainer));
//}

void toadll::c_Entity::setRotationYaw(float newYaw) const
{
	env->SetFloatField(obj, get_fid(obj, mappingFields::rotationYawField, env), newYaw);
	//env->CallVoidMethod(obj, get_mid(obj, mapping::setRotationYaw), newYaw);
}

void toadll::c_Entity::setPrevRotationYaw(float newYaw) const
{
	env->SetFloatField(obj, get_fid(obj, mappingFields::prevRotationYawField, env), newYaw);
	//env->CallVoidMethod(obj, get_mid(obj, mapping::setRotationYaw), newYaw);
}

void toadll::c_Entity::setRotationPitch(float newPitch) const
{
	env->SetFloatField(obj, get_fid(obj, mappingFields::rotationPitchField, env), newPitch);
	//env->CallVoidMethod(obj, get_mid(obj, mapping::setRotationPitch), newPitch);
}

void toadll::c_Entity::setPrevRotationPitch(float newPitch) const
{
	env->SetFloatField(obj, get_fid(obj, mappingFields::prevRotationPitchField, env), newPitch);
	//env->CallVoidMethod(obj, get_mid(obj, mapping::setRotationPitch), newPitch);
}

void toadll::c_Entity::setRotation(float yaw, float pitch) const
{
	env->CallVoidMethod(obj, get_mid(obj, mapping::setRotation, env), yaw, pitch);
}

void toadll::c_Entity::setMotionX(float val) const
{
	env->SetDoubleField(obj, get_fid(obj, mappingFields::motionXField, env), val);
}

void toadll::c_Entity::setMotionY(float val) const
{
	env->SetDoubleField(obj, get_fid(obj, mappingFields::motionYField, env), val);
}

void toadll::c_Entity::setMotionZ(float val) const
{
	env->SetDoubleField(obj, get_fid(obj, mappingFields::motionZField, env), val);
}
