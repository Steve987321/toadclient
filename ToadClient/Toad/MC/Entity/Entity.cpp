#include "pch.h"
#include "Toad/Toad.h"
#include "Entity.h"

toadll::c_Entity::~c_Entity()
{
	env->DeleteLocalRef(obj);
}

toadll::vec3 toadll::c_Entity::getPosition() const
{
	//static bool once = false;
	//if (!once)
	//{
	//	loop_through_class(elclass, env);
	//	once = true;
	//}
	auto mId = toad::g_curr_client == toad::minecraft_client::Lunar_189
		? get_mid(obj, mapping::getPos, env)
		: get_mid(elclass, mapping::getPos, env);

	if (!mId)
		return { 0,0,0 };

	auto vecobj = env->CallObjectMethod(obj, mId);
	if (!vecobj)
	{
		return { 0,0,0 };
	}
	auto ret = to_vec3(vecobj, env);
	env->DeleteLocalRef(vecobj);
	return ret;
}

toadll::vec3 toadll::c_Entity::getLastTickPosition() const
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
	if (!mId) // no item held 
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

	auto mainInv = env->CallObjectMethod(inventory, mId, slot);
	env->DeleteLocalRef(inventory);

	if (!mainInv) // no item held
	{
		return "NONE";
	}

	auto juice = env->GetObjectClass(mainInv);
	loop_through_class(juice, env);
	env->DeleteLocalRef(juice);

	env->DeleteLocalRef(mainInv);
	return "";
}

int toadll::c_Entity::getHurtTime() const
{
	auto mId = get_mid(elclass, mapping::getHurtTime, env);
	if (!mId)
		return 0;
	auto res = env->CallIntMethod(obj, mId);
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
	return env->GetDoubleField(obj, fId);
}

float toadll::c_Entity::getMotionY() const
{
	//return env->CallDoubleMethod(obj, get_mid(obj, mapping::getMotionY));
	auto fId = get_fid(obj, mappingFields::motionYField, env);
	if (!fId)
		return 0;
	return env->GetDoubleField(obj, fId);
}

float toadll::c_Entity::getMotionZ() const
{
	//return env->CallDoubleMethod(obj, get_mid(obj, mapping::getMotionZ));
	auto fId = get_fid(obj, mappingFields::motionZField, env);
	if (!fId)
		return 0;
	return env->GetDoubleField(obj, fId);
}

bool toadll::c_Entity::isInvisible() const
{
	auto mId = get_mid(obj, mapping::isInvisible, env);
	if (!mId)
		return false;
	return env->CallBooleanMethod(obj, mId);
}

toadll::bbox toadll::c_Entity::get_BBox() const
{
	auto bboxobj = env->CallObjectMethod(obj, get_mid(obj, mapping::getBBox, env));

	float minX = env->CallDoubleMethod(bboxobj, get_mid(bboxobj, mapping::bboxMinX, env));
	float minY = env->CallDoubleMethod(bboxobj, get_mid(bboxobj, mapping::bboxMinY, env));
	float minZ = env->CallDoubleMethod(bboxobj, get_mid(bboxobj, mapping::bboxMinZ, env));
	float maxX = env->CallDoubleMethod(bboxobj, get_mid(bboxobj, mapping::bboxMaxX, env));
	float maxY = env->CallDoubleMethod(bboxobj, get_mid(bboxobj, mapping::bboxMaxY, env));
	float maxZ = env->CallDoubleMethod(bboxobj, get_mid(bboxobj, mapping::bboxMaxZ, env));

	env->DeleteLocalRef(bboxobj);

	return { { minX, minY, minZ }, { maxX, maxY, maxZ } };
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
