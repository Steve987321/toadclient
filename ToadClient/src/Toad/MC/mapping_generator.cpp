#include "pch.h"
#include "Toad/toadll.h"
#include "mappings.h"
#include "mapping_generator.h"

#include "nlohmann/json.hpp"

#define JVMTICALL(f, ...)													\
if (auto res = f(__VA_ARGS__); res != jvmtiError::JVMTI_ERROR_NONE)			\
{																			\
	LOGERROR("[MappingGenerator] Failed to call {}, {}", #f, (int)res);		\
	return;																	\
}

namespace toadll
{

using json = nlohmann::json;

void MappingGenerator::Generate(JNIEnv* env, jvmtiEnv* jvmti_env)
{
	json data;
	json fields_data;
	json methods_data;
	json mc_data;

	CHAR documents[MAX_PATH];
	HRESULT get_folder_path_res = SHGetFolderPathA(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, documents);
	if (get_folder_path_res != S_OK)
	{
		LOGERROR("[MappingGenerator] Failed to get documents folder");
		return;
	}

	std::ofstream file(std::filesystem::path(documents) / "mapping_gen_out.txt");
	if (!file)
	{
		LOGERROR("[MappingGenerator] Failed to create output file");
		return;
	}

	jclass mc = Minecraft::findMcClass(env);
	if (!mc)
	{
		LOGDEBUG("[MappingGenerator] Minecraft class not found");
		file.close();
		return;
	}

	jint field_count = 0;
	jfieldID* fields;

	jvmtiError res;
	//JVMTICALL(jvmti_env->GetClassFields, mc, &field_count, &fields)
	res = jvmti_env->GetClassFields(mc, &field_count, &fields);
	if (res != jvmtiError::JVMTI_ERROR_NONE)
	{
		LOGERROR("[MappingGenerator] Failed to call GetClassFields, {}", (int)res);
		env->DeleteLocalRef(mc);
		return; 
	}

	LOGDEBUG("[MappingGenerator] Fields");
	for (int i = 0; i < field_count; i++)
	{
		char* name = nullptr;
		char* sig = nullptr;
		char* generic = nullptr;
		jint modifiers = 0;

		if (res = jvmti_env->GetFieldName(mc, fields[i], &name, &sig, &generic); res != JVMTI_ERROR_NONE)
		{
			LOGERROR("[MappingGenerator] Failed to call GetFieldName, {}", (int)res);
			continue;
		}
		if (res = jvmti_env->GetFieldModifiers(mc, fields[i], &modifiers); res != JVMTI_ERROR_NONE)
		{
			LOGERROR("[MappingGenerator] Failed to call GetFieldModifiers, {}", (int)res);
			continue;
		}
		
		json serialized;
		serialized["name"] = name ? name : "null";
		serialized["sig"] = sig ? sig : "null";
		//serialized["generic"] = generic ? generic : "null";
		serialized["modifiers"] = modifiers;

		fields_data[i] = serialized;
	}

	LOGDEBUG("[MappingGenerator] Fields done");
	jvmti_env->Deallocate((unsigned char*)fields);

	LOGDEBUG("[MappingGenerator] Methods");

	jint method_count = 0;
	jmethodID* methods;
	res = jvmti_env->GetClassMethods(mc, &method_count, &methods);
	if (res != jvmtiError::JVMTI_ERROR_NONE)
	{
		LOGERROR("[MappingGenerator] Failed to call GetClassMethods, {}", (int)res);
		env->DeleteLocalRef(mc);
		return;
	}
	std::vector<unsigned char> bytecodes_vec;

	for (int i = 0; i < method_count; i++)
	{
		jint bytecodes_count = 0;
		jint modifiers = 0;
		char* name = nullptr;
		char* sig = nullptr;
		char* generic = nullptr;
		unsigned char* bytecodes = nullptr;

		if (res = jvmti_env->GetBytecodes(methods[i], &bytecodes_count, &bytecodes); res != JVMTI_ERROR_NONE)
		{
			LOGERROR("[MappingGenerator] Failed to call GetBytecodes, {}", (int)res);
			continue;
		}
		if (res = jvmti_env->GetMethodModifiers(methods[i], &modifiers); res != JVMTI_ERROR_NONE)
		{
			LOGERROR("[MappingGenerator] Failed to call GetMethodModifiers, {}", (int)res);
			continue;
		}
		if (res = jvmti_env->GetMethodName(methods[i], &name, & sig, &generic); res != JVMTI_ERROR_NONE)
		{
			LOGERROR("[MappingGenerator] Failed to call GetMethodName, {}", (int)res);
			continue;
		}

		bytecodes_vec.clear();
		bytecodes_vec.reserve(bytecodes_count);
		for (int j = 0; j < bytecodes_count; j++)
			bytecodes_vec.emplace_back(bytecodes[j]);

		json serialized;
		serialized["name"] = name ? name : "null";
		serialized["sig"] = sig ? sig : "null";
		//serialized["generic"] = generic ? generic : "null";
		serialized["modifiers"] = modifiers;
		serialized["bytecodes"] = bytecodes_vec;
		methods_data[i] = serialized;
	}

	jvmti_env->Deallocate((unsigned char*)methods);
	env->DeleteLocalRef(mc);

	// get class index 

	jint class_count = 0;
	jclass* classes;
	res = jvmti_env->GetLoadedClasses(&class_count, &classes);
	if (res != JVMTI_ERROR_NONE)
	{
		env->DeleteLocalRef(mc);
		return;
	}

	int class_index = -1;
	for (int i = 0; i < class_count; i++)
	{
		bool same_field_count = jvmfunc::oJVM_GetClassFieldsCount(env, classes[i]) == field_count;
		bool same_method_count = jvmfunc::oJVM_GetClassMethodsCount(env, classes[i]) == method_count;
		if (same_field_count && same_method_count)
		{
			class_index = i;
			break;
		}
	}

	jvmti_env->Deallocate((unsigned char*)classes);
	env->DeleteLocalRef(mc);

	json minecraft_data;
	minecraft_data["mc_class_index"] = class_index;
	minecraft_data["methods"] = methods_data;
	minecraft_data["fields"] = fields_data;

	data["minecraft"] = minecraft_data;

	file << data << std::endl;
	file.close();
}

void MappingGenerator::UpdateFile(JNIEnv* env, jvmtiEnv* jvmti_env, const std::filesystem::path& json_file)
{
	std::fstream file(json_file);

	std::stringstream ss;
	ss << file.rdbuf();
	json data;

	try
	{
		data = json::parse(ss.str());
	}
	catch (json::parse_error& e)
	{
		LOGERROR("[MappingGenerator] json parse error: {}", e.what());
		return;
	}

	jclass mc = Minecraft::findMcClass(env);
	if (!mc)
	{
		LOGDEBUG("[MappingGenerator] Minecraft class not found");
		file.close();
		return;
	}

	jint field_count = 0;
	jfieldID* fields;

	jvmtiError res;
	res = jvmti_env->GetClassFields(mc, &field_count, &fields);
	if (res != jvmtiError::JVMTI_ERROR_NONE)
	{
		LOGERROR("[MappingGenerator] Failed to call GetClassFields, {}", (int)res);
		env->DeleteLocalRef(mc);
		return;
	}

	for (int i = 0; i < field_count; i++)
	{
		fields[i];
	}

}

void MappingGenerator::GetMappingsFromFile(JNIEnv* env, jvmtiEnv* jvmti_env, const std::filesystem::path& json_file)
{
	std::ifstream file(json_file);

	std::stringstream ss;
	ss << file.rdbuf();
	json data;

	try
	{
		data = json::parse(ss.str());
	}
	catch (json::parse_error& e)
	{
		LOGERROR("[MappingGenerator] json parse error: {}", e.what());
		return;
	}

	json minecraft_data = data.at("minecraft");
	int index = minecraft_data.at("mc_class_index");
	json methods_data = minecraft_data.at("methods");
	json fields_data = minecraft_data.at("fields");
	

	// find mc class
	jint class_count;
	jclass* classes;
	jvmti_env->GetLoadedClasses(&class_count, &classes);

	jclass mc_class = nullptr;

	std::set<jclass> scanned;
	for (int i = 0; i < class_count; i++)
	{
		int fields_count = jvmfunc::oJVM_GetClassFieldsCount(env, classes[i]);
		int methods_count = jvmfunc::oJVM_GetClassMethodsCount(env, classes[i]);
		
		bool fields_close_enough = abs(fields_count - (int)fields_data.size()) <= 15;
		bool methods_close_enough = abs(methods_count - (int)methods_data.size()) <= 15;

		if (fields_close_enough && methods_close_enough)
		{ 
			for (int j = 0; j < fields_count; j++)
			{

			}
			
			mc_class = classes[i];
		}
	}

	//for (int i = 0; i < class_count; i++)
	//{
	//	jvmti_env->GetClassFields();
	//}

}

}