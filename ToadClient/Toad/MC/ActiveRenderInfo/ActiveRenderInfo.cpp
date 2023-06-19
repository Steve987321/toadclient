#include "pch.h"
#include "Toad/Toad.h"
#include "ActiveRenderInfo.h"

std::vector<float> toadll::c_ActiveRenderInfo::get_modelview() const
{
	std::vector<float> res = {}; 
	auto fid = get_static_fid(ariclass, mappingFields::modelviewField, env);
	auto obj = env->GetStaticObjectField(ariclass, fid);
	auto bufklass = env->GetObjectClass(obj);
	auto getIndexBuf = env->GetMethodID(bufklass, "get", "(I)F");

	res.reserve(16);
	for (int i = 0; i < 16; i++)
	{
		res.emplace_back(env->CallFloatMethod(obj, getIndexBuf, i));
	}

	env->DeleteLocalRef(obj);
	env->DeleteLocalRef(bufklass);
	return res;
}

void toadll::c_ActiveRenderInfo::set_modelview(std::vector<float>& vec) const
{
	vec.reserve(16);
	auto fid = get_static_fid(ariclass, mappingFields::modelviewField, env);
	auto obj = env->GetStaticObjectField(ariclass, fid);
	auto bufklass = env->GetObjectClass(obj);
	auto getIndexBuf = env->GetMethodID(bufklass, "get", "(I)F");

	for (int i = 0; i < 16; i++)
	{
		vec[i] = env->CallFloatMethod(obj, getIndexBuf, i);
	}

	env->DeleteLocalRef(obj);
	env->DeleteLocalRef(bufklass);
}

std::vector<float>  toadll::c_ActiveRenderInfo::get_projection() const
{
	std::vector<float> res = {};
	auto fid = get_static_fid(ariclass, mappingFields::projectionField, env);
	auto obj = env->GetStaticObjectField(ariclass, fid);
	auto bufklass = env->GetObjectClass(obj);
	auto getIndexBuf = env->GetMethodID(bufklass, "get", "(I)F");

	res.reserve(16);
	for (int i = 0; i < 16; i++)
	{
		res.emplace_back(env->CallFloatMethod(obj, getIndexBuf, i));
	}

	env->DeleteLocalRef(obj);
	env->DeleteLocalRef(bufklass);
	return res;
}

void toadll::c_ActiveRenderInfo::set_projection(std::vector<float>& vec) const
{
	vec.reserve(16);
	auto fid = get_static_fid(ariclass, mappingFields::projectionField, env);
	auto obj = env->GetStaticObjectField(ariclass, fid);
	auto bufklass = env->GetObjectClass(obj);
	auto getIndexBuf = env->GetMethodID(bufklass, "get", "(I)F");

	for (int i = 0; i < 16; i++)
	{
		vec[i] = env->CallFloatMethod(obj, getIndexBuf, i);
	}

	env->DeleteLocalRef(obj);
	env->DeleteLocalRef(bufklass);
}

//void toadll::c_ActiveRenderInfo::get_viewport(GLint viewportBuf[4]) const
//{
//	auto fid = get_static_fid(ariclass, mappingFields::viewportField);
//	auto obj = env->GetStaticObjectField(ariclass, fid);
//	auto ibufklass = env->GetObjectClass(obj);
//	auto getIndexBuf = env->GetMethodID(ibufklass, "get", "(I)I");
//
//	for (int i = 0; i < 4; i++)
//	{
//		viewportBuf[i] = env->CallIntMethod(obj, getIndexBuf, i);
//	}
//
//	env->DeleteLocalRef(obj);
//	env->DeleteLocalRef(ibufklass);
//}

toadll::vec3 toadll::c_ActiveRenderInfo::get_render_pos() const
{
	return to_vec3(env->CallStaticObjectMethod(ariclass, get_static_mid(ariclass, mapping::getRenderPos, env)), env);
}
