#include "pch.h"
#include "Toad/Toad.h"
#include "ActiveRenderInfo.h"

void toadll::c_ActiveRenderInfo::get_modelview(GLint* viewportbuf) const
{	
	auto fid = get_static_fid(ariclass, mappingFields::modelviewField);
	auto obj = env->GetStaticObjectField(ariclass, fid);
	auto ibufklass = env->GetObjectClass(obj);
	auto getIndexBuf = env->GetMethodID(ibufklass, "get", "(I)F");

	for (int i = 0; i < 16; i++)
	{
		viewportbuf[i] = env->CallIntMethod(obj, getIndexBuf, i);
	}

	env->DeleteLocalRef(obj);
	env->DeleteLocalRef(ibufklass);
}

void toadll::c_ActiveRenderInfo::get_projection(GLfloat* projectionbuf) const
{
	auto fid = get_static_fid(ariclass, mappingFields::modelviewField);
	auto obj = env->GetStaticObjectField(ariclass, fid);
	auto ibufklass = env->GetObjectClass(obj);
	auto getIndexBuf = env->GetMethodID(ibufklass, "get", "(I)F");

	for (int i = 0; i < 16; i++)
	{
		projectionbuf[i] = env->CallFloatMethod(obj, getIndexBuf, i);
	}

	env->DeleteLocalRef(obj);
	env->DeleteLocalRef(ibufklass);
}

void toadll::c_ActiveRenderInfo::get_viewport(GLfloat* viewportBuf) const
{
	auto fid = get_static_fid(ariclass, mappingFields::modelviewField);
	auto obj = env->GetStaticObjectField(ariclass, fid);
	auto ibufklass = env->GetObjectClass(obj);
	auto getIndexBuf = env->GetMethodID(ibufklass, "get", "(I)I");

	for (int i = 0; i < 4; i++)
	{
		viewportBuf[i] = env->CallFloatMethod(obj, getIndexBuf, i);
	}

	env->DeleteLocalRef(obj);
	env->DeleteLocalRef(ibufklass);
}

toadll::vec3 toadll::c_ActiveRenderInfo::get_render_pos() const
{
	return to_vec3(env->CallStaticObjectMethod(ariclass, get_static_mid(ariclass, mapping::getRenderPos)));
}
