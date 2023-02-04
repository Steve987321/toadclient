#include "pch.h"
#include "Toad/Toad.h"
#include "Entity.h"

toadll::c_Entity::~c_Entity()
{
	if (*obj != nullptr)
		env->DeleteLocalRef(*obj);
}

toadll::vec3 toadll::c_Entity::get_position() const
{
	return to_vec3(env->CallObjectMethod(*obj, get_mid(*obj, mapping::getPos)));
}

float toadll::c_Entity::get_rotationYaw() const
{
	return env->CallDoubleMethod(*obj, get_mid(*obj, mapping::getRotationYaw));
}

float toadll::c_Entity::get_rotationPitch() const
{
	return env->CallDoubleMethod(*obj, get_mid(*obj, mapping::getRotationPitch));
}

std::string toadll::c_Entity::get_name() const
{
	return jstring2string((jstring)env->CallObjectMethod(*obj, get_mid(*obj, mapping::getName)));
}

jobject toadll::c_Entity::get_heldItem() const
{
	auto mid = get_mid(*obj, mapping::getHeldItem);
	if (mid == NULL) // no item held 
		return nullptr;
	return env->CallObjectMethod(*obj, mid);
}

void toadll::c_Entity::set_rotationYaw(float newYaw) const
{
	env->CallVoidMethod(*obj, get_mid(*obj, mapping::setRotationYaw), newYaw);
}

void toadll::c_Entity::set_rotationPitch(float newPitch) const
{
	env->CallVoidMethod(*obj, get_mid(*obj, mapping::setRotationPitch), newPitch);
}

void toadll::c_Entity::set_rotation(float yaw, float pitch) const
{
	env->CallVoidMethod(*obj, get_mid(*obj, mapping::setRotation), yaw, pitch);
}