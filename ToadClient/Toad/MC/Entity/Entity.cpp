#include "pch.h"
#include "Toad/Toad.h"
#include "Entity.h"

toadll::c_Entity::~c_Entity()
{
	env->DeleteLocalRef(obj);
}

toadll::vec3 toadll::c_Entity::get_position() const
{
	auto vecobj = env->CallObjectMethod(obj, get_mid(obj, mapping::getPos));
	auto ret = to_vec3(vecobj);
	env->DeleteLocalRef(vecobj);
	return ret;
}

float toadll::c_Entity::get_rotationYaw() const
{
	return env->CallDoubleMethod(obj, get_mid(obj, mapping::getRotationYaw));
}

float toadll::c_Entity::get_rotationPitch() const
{
	return env->CallDoubleMethod(obj, get_mid(obj, mapping::getRotationPitch));
}

std::string toadll::c_Entity::get_name() const
{
	auto strobj = env->CallObjectMethod(obj, get_mid(obj, mapping::getName));
	auto ret = jstring2string((jstring)strobj);
	env->DeleteLocalRef(strobj);
	return ret;
}

jobject toadll::c_Entity::get_heldItem() const
{
	auto mid = get_mid(obj, mapping::getHeldItem);
	if (mid == NULL) // no item held 
		return nullptr;
	return env->CallObjectMethod(obj, mid);
}

float toadll::c_Entity::get_motionX() const
{
	return env->CallDoubleMethod(obj, get_mid(obj, mapping::getMotionX));
}

float toadll::c_Entity::get_motionY() const
{
	return env->CallDoubleMethod(obj, get_mid(obj, mapping::getMotionY));
}

float toadll::c_Entity::get_motionZ() const
{
	return env->CallDoubleMethod(obj, get_mid(obj, mapping::getMotionZ));
}

//jobject toadll::c_Entity::get_open_container() const
//{
//	return env->CallObjectMethod(obj, get_mid(obj, mapping::getOpenContainer));
//}

void toadll::c_Entity::set_rotationYaw(float newYaw) const
{
	env->SetFloatField(obj, get_fid(obj, mapping::rotationYawField), newYaw);
	//env->CallVoidMethod(obj, get_mid(obj, mapping::setRotationYaw), newYaw);
}

void toadll::c_Entity::set_prevRotationYaw(float newYaw) const
{
	env->SetFloatField(obj, get_fid(obj, mapping::prevRotationYawField), newYaw);
	//env->CallVoidMethod(obj, get_mid(obj, mapping::setRotationYaw), newYaw);
}

void toadll::c_Entity::set_rotationPitch(float newPitch) const
{
	env->SetFloatField(obj, get_fid(obj, mapping::rotationPitchField), newPitch);
	//env->CallVoidMethod(obj, get_mid(obj, mapping::setRotationPitch), newPitch);
}

void toadll::c_Entity::set_prevRotationPitch(float newPitch) const
{
	env->SetFloatField(obj, get_fid(obj, mapping::prevRotationPitchField), newPitch);
	//env->CallVoidMethod(obj, get_mid(obj, mapping::setRotationPitch), newPitch);
}

void toadll::c_Entity::set_rotation(float yaw, float pitch) const
{
	env->CallVoidMethod(obj, get_mid(obj, mapping::setRotation), yaw, pitch);
}