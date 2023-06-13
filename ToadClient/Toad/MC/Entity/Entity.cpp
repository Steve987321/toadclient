#include "pch.h"
#include "Toad/Toad.h"
#include "Entity.h"

toadll::c_Entity::~c_Entity()
{
	env->DeleteLocalRef(obj);
}

toadll::vec3 toadll::c_Entity::getPosition() const
{
	auto vecobj = env->CallObjectMethod(obj, get_mid(obj, mapping::getPos, env));
	auto ret = to_vec3(vecobj, env);
	env->DeleteLocalRef(vecobj);
	return ret;
}

toadll::vec3 toadll::c_Entity::getLastTickPosition() const
{
	return {
		static_cast<float>(env->GetDoubleField(obj, get_fid(obj, mappingFields::lastTickPosXField, env))),
		static_cast<float>(env->GetDoubleField(obj, get_fid(obj, mappingFields::lastTickPosYField, env))),
		static_cast<float>(env->GetDoubleField(obj, get_fid(obj, mappingFields::lastTickPosZField, env))),
	};
}

float toadll::c_Entity::getRotationYaw() const
{
	//return env->CallDoubleMethod(obj, get_mid(obj, mapping::getRotationYaw, env));
	return env->GetFloatField(obj, get_fid(obj, mappingFields::rotationYawField, env));
}

float toadll::c_Entity::getRotationPitch() const
{
	//return env->CallDoubleMethod(obj, get_mid(obj, mapping::getRotationPitch, env));
	return env->GetFloatField(obj, get_fid(obj, mappingFields::rotationPitchField, env));
}

std::string toadll::c_Entity::getName() const
{
	auto strobj = env->CallObjectMethod(obj, get_mid(obj, mapping::getName, env));
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
	auto mid = get_mid(obj, mapping::getHeldItem, env);
	if (mid == NULL) // no item held 
		return nullptr;
	return env->CallObjectMethod(obj, mid);
}

std::string toadll::c_Entity::getSlotStr(int slot) const
{
	auto inventory = env->GetObjectField(obj, get_fid(obj, mappingFields::inventoryField, env));
	auto mid = get_mid(inventory, mapping::getStackInSlot, env);

	auto mainInv = env->CallObjectMethod(inventory, mid, slot);
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
	auto mid = get_mid(elclass, mapping::getHurtTime, env);
	return env->CallIntMethod(obj, mid);
}

float toadll::c_Entity::getHealth() const
{
	auto mid = get_mid(elclass, mapping::getHealth, env);
	return env->CallFloatMethod(obj, mid);
}

float toadll::c_Entity::getMotionX() const
{
	//return env->CallDoubleMethod(obj, get_mid(obj, mapping::getMotionX));
	return env->GetDoubleField(obj, get_fid(obj, mappingFields::motionXField, env));
}

float toadll::c_Entity::getMotionY() const
{
	//return env->CallDoubleMethod(obj, get_mid(obj, mapping::getMotionY));
	return env->GetDoubleField(obj, get_fid(obj, mappingFields::motionYField, env));
}

float toadll::c_Entity::getMotionZ() const
{
	//return env->CallDoubleMethod(obj, get_mid(obj, mapping::getMotionZ));
	return env->GetDoubleField(obj, get_fid(obj, mappingFields::motionZField, env));
}

bool toadll::c_Entity::isInvisible() const
{
	return env->CallBooleanMethod(obj, get_mid(obj, mapping::isInvisible, env));
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

	return {{ minX, minY, minZ }, { maxX, maxY, maxZ } };
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
