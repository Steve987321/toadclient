#include "pch.h"
#include "Toad/toad.h"
#include "active_render_info.h"

namespace toad
{

	void ActiveRenderInfo::getModelView(std::array<double, 16>& arr) const
	{
		auto fid = get_static_fid(ariclass, mappingFields::modelviewField, env);
		if (!fid)
			return;

		auto obj = env->GetStaticObjectField(ariclass, fid);
		auto bufklass = env->GetObjectClass(obj);
		static auto getIndexBuf = env->GetMethodID(bufklass, "get", "(I)F");
		for (int i = 0; i < 16; i++)
		{
			arr[i] = (double)env->CallFloatMethod(obj, getIndexBuf, i);
		}

		env->DeleteLocalRef(obj);
		env->DeleteLocalRef(bufklass);
	}

	void ActiveRenderInfo::getProjection(std::array<double, 16>& arr) const
	{
		auto fid = get_static_fid(ariclass, mappingFields::projectionField, env);
		if (!fid)
			return;
		auto obj = env->GetStaticObjectField(ariclass, fid);
		auto bufklass = env->GetObjectClass(obj);
		auto getIndexBuf = env->GetMethodID(bufklass, "get", "(I)F");

		for (int i = 0; i < 16; i++)
		{
			arr[i] = (double)env->CallFloatMethod(obj, getIndexBuf, i);
		}

		env->DeleteLocalRef(obj);
		env->DeleteLocalRef(bufklass);
	}

	//void c_ActiveRenderInfo::get_viewport(GLint viewportBuf[4]) const
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

	//Vec3 ActiveRenderInfo::get_render_pos() const
	//{
	//	auto mid = get_static_mid(ariclass, mapping::getRenderPos, env);
	//	if (!mid)
	//		return {};
	//
	//	auto obj = env->CallStaticObjectMethod(ariclass, mid);
	//
	//	auto res = to_vec3(obj, env);
	//
	//	env->DeleteLocalRef(obj);
	//
	//	return res;
	//}

}
