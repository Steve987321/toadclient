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
	std::cout << "f S: " <<  env->GetObjectClass(*obj) << std::endl;
	Sleep(1000);
	std::cout << "this is : " << env->GetMethodID(env->GetObjectClass(*obj), "getPositionVector", "()Lnet/minecraft/util/Vec3;");
	Sleep(2000);
	auto position = env->CallObjectMethod(*obj, get_mid(*obj, mapping::getPos));
	std::cout << "position obj vector3: " << position << std::endl;
	Sleep(2000);
	return to_vec3(position);
}

float toadll::c_Entity::get_rotationYaw() const
{
	return env->CallFloatMethod(*obj, get_mid(*obj, mapping::getRotationYaw));
}

float toadll::c_Entity::get_rotationPitch() const
{
	return env->CallFloatMethod(*obj, get_mid(*obj, mapping::getRotationPitch));
}

void toadll::c_Entity::set_rotationYaw(float newYaw) const
{
	env->CallVoidMethod(*obj, get_mid(*obj, mapping::setRotationYaw), newYaw);
}

void toadll::c_Entity::set_rotationPitch(float newPitch) const
{
	env->CallVoidMethod(*obj, get_mid(*obj, mapping::setRotationPitch), newPitch);
}
