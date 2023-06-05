#include "pch.h"
#include "Toad/Toad.h"
#include "Entity.h"

toadll::c_Entity::~c_Entity()
{
	env->DeleteLocalRef(obj);
}

toadll::vec3 toadll::c_Entity::get_position() const
{
	auto vecobj = env->CallObjectMethod(obj, get_mid(obj, mapping::getPos, env));
	auto ret = to_vec3(vecobj, env);
	env->DeleteLocalRef(vecobj);
	return ret;
}

toadll::vec3 toadll::c_Entity::get_lasttickposition() const
{
	return {
		static_cast<float>(env->GetDoubleField(obj, get_fid(obj, mappingFields::lastTickPosXField, env))),
		static_cast<float>(env->GetDoubleField(obj, get_fid(obj, mappingFields::lastTickPosYField, env))),
		static_cast<float>(env->GetDoubleField(obj, get_fid(obj, mappingFields::lastTickPosZField, env))),
	};
}

float toadll::c_Entity::get_rotationYaw() const
{
	return env->CallDoubleMethod(obj, get_mid(obj, mapping::getRotationYaw, env));
}

float toadll::c_Entity::get_rotationPitch() const
{
	return env->CallDoubleMethod(obj, get_mid(obj, mapping::getRotationPitch, env));
}

std::string toadll::c_Entity::get_name() const
{
	auto strobj = env->CallObjectMethod(obj, get_mid(obj, mapping::getName, env));
	auto ret = jstring2string((jstring)strobj, env);
	env->DeleteLocalRef(strobj);
	return ret;
}

std::string toadll::c_Entity::get_heldItemStr() const
{
	auto item = get_heldItem();
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

jobject toadll::c_Entity::get_heldItem() const
{
	auto mid = get_mid(obj, mapping::getHeldItem, env);
	if (mid == NULL) // no item held 
		return nullptr;
	return env->CallObjectMethod(obj, mid);
}

int toadll::c_Entity::get_hurt_time() const
{
	auto mid = get_mid(elclass, mapping::getHurtTime, env);
	return env->CallIntMethod(obj, mid);
}

float toadll::c_Entity::get_motionX() const
{
	//return env->CallDoubleMethod(obj, get_mid(obj, mapping::getMotionX));
	return env->GetDoubleField(obj, get_fid(obj, mappingFields::motionXField, env));
}

float toadll::c_Entity::get_motionY() const
{
	//return env->CallDoubleMethod(obj, get_mid(obj, mapping::getMotionY));
	return env->GetDoubleField(obj, get_fid(obj, mappingFields::motionYField, env));
}

float toadll::c_Entity::get_motionZ() const
{
	//return env->CallDoubleMethod(obj, get_mid(obj, mapping::getMotionZ));
	return env->GetDoubleField(obj, get_fid(obj, mappingFields::motionZField, env));
}

bool toadll::c_Entity::is_invisible() const
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

void toadll::c_Entity::set_rotationYaw(float newYaw) const
{
	env->SetFloatField(obj, get_fid(obj, mappingFields::rotationYawField, env), newYaw);
	//env->CallVoidMethod(obj, get_mid(obj, mapping::setRotationYaw), newYaw);
}

void toadll::c_Entity::set_prevRotationYaw(float newYaw) const
{
	env->SetFloatField(obj, get_fid(obj, mappingFields::prevRotationYawField, env), newYaw);
	//env->CallVoidMethod(obj, get_mid(obj, mapping::setRotationYaw), newYaw);
}

void toadll::c_Entity::set_rotationPitch(float newPitch) const
{
	env->SetFloatField(obj, get_fid(obj, mappingFields::rotationPitchField, env), newPitch);
	//env->CallVoidMethod(obj, get_mid(obj, mapping::setRotationPitch), newPitch);
}

void toadll::c_Entity::set_prevRotationPitch(float newPitch) const
{
	env->SetFloatField(obj, get_fid(obj, mappingFields::prevRotationPitchField, env), newPitch);
	//env->CallVoidMethod(obj, get_mid(obj, mapping::setRotationPitch), newPitch);
}

void toadll::c_Entity::set_rotation(float yaw, float pitch) const
{
	env->CallVoidMethod(obj, get_mid(obj, mapping::setRotation, env), yaw, pitch);
}

void toadll::c_Entity::set_motionX(float val) const
{
	env->SetDoubleField(obj, get_fid(obj, mappingFields::motionXField, env), val);
}

void toadll::c_Entity::set_motionY(float val) const
{
	env->SetDoubleField(obj, get_fid(obj, mappingFields::motionYField, env), val);
}

void toadll::c_Entity::set_motionZ(float val) const
{
	env->SetDoubleField(obj, get_fid(obj, mappingFields::motionZField, env), val);
}
