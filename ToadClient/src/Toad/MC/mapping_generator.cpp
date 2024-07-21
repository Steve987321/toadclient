#include "pch.h"
#include "Toad/toadll.h"
#include "mappings.h"
#include "mapping_generator.h"

#include "nlohmann/json.hpp"

#include "../../Loader/src/Application/config.h"

#define JVMTICALL(f, ...)													\
if (auto res = f(__VA_ARGS__); res != jvmtiError::JVMTI_ERROR_NONE)			\
{																			\
	LOGERROR("[MappingGenerator] Failed to call {}, {}", #f, (int)res);		\
	return;																	\
}

namespace toadll
{

using json = nlohmann::json;

json Mappings::Serialize()
{
	json data;
	json methods_data;
	json fields_data;

	for (int i = 0; i < fields.size(); i++)
	{
		json field_data;

		field_data["modifiers"] = fields[i].modifiers;
		field_data["signature"] = fields[i].signature;
		field_data["name"] = fields[i].name;

		fields_data[i] = field_data;
	}

	for (int i = 0; i < methods.size(); i++)
	{
		json method_data;

		method_data["modifiers"] = methods[i].modifiers;
		method_data["signature"] = methods[i].signature;
		method_data["name"] = methods[i].name;
		method_data["bytecodes"] = methods[i].bytecodes;
		
		methods_data[i] = method_data;
	}

	data["methods"] = methods_data;
	data["fields"] = fields_data;
	return data;
}

Mappings Mappings::Deserialize(const json& data)
{
	Mappings mappings;
	json methods_data;
	json fields_data;

	config::get_json_element(methods_data, data, "methods");
	config::get_json_element(fields_data, data, "fields");

	for (int i = 0; i < methods_data.size(); i++)
	{
		MethodMapping method;
		json method_data = methods_data[i];
		config::get_json_element(method.name, method_data, "name");
		config::get_json_element(method.signature, method_data, "signature");
		config::get_json_element(method.modifiers, method_data, "modifiers");
		config::get_json_element(method.bytecodes, method_data, "bytecodes");

		mappings.methods.emplace_back(method);
	}

	for (int i = 0; i < fields_data.size(); i++)
	{
		MappingField field;
		json method_data = methods_data[i];
		config::get_json_element(field.name, method_data, "name");
		config::get_json_element(field.signature, method_data, "signature");
		config::get_json_element(field.modifiers, method_data, "modifiers");
		mappings.fields.emplace_back(field);
	}
	
	return mappings;
}

void MappingGenerator::Generate(JNIEnv* env, jvmtiEnv* jvmti_env)
{
	json data;

	CHAR documents[MAX_PATH];
	HRESULT get_folder_path_res = SHGetFolderPathA(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, documents);
	if (get_folder_path_res != S_OK)
	{
		LOGERROR("[MappingGenerator] Failed to get documents folder");
		return;
	}

	jclass mc = Minecraft::findMcClass(env);
	if (!mc)
	{
		LOGDEBUG("[MappingGenerator] Minecraft class not found");
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
	Mappings mc_mappings;

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

		MappingField field;
		field.name = name;
		field.signature = sig;
		field.modifiers = modifiers;
		mc_mappings.fields.emplace_back(field);
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
		if (res = jvmti_env->GetMethodName(methods[i], &name, &sig, &generic); res != JVMTI_ERROR_NONE)
		{
			LOGERROR("[MappingGenerator] Failed to call GetMethodName, {}", (int)res);
			continue;
		}

		bytecodes_vec.clear();
		bytecodes_vec.reserve(bytecodes_count);
		for (int j = 0; j < bytecodes_count; j++)
			bytecodes_vec.emplace_back(bytecodes[j]);

		MethodMapping method;
		method.name = name;
		method.signature = sig;
		method.modifiers = modifiers;
		method.bytecodes = bytecodes_vec;
		mc_mappings.methods.emplace_back(method);
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
	data["mappings"] = mc_mappings.Serialize();
	std::ofstream file(std::filesystem::path(documents) / "mapping_gen_out.txt");
	if (!file)
	{
		LOGERROR("[MappingGenerator] Failed to create output file");
		return;
	}
	file << data << std::endl;
	file.close();
}

void MappingGenerator::UpdateFile(JNIEnv* env, jvmtiEnv* jvmti_env, const std::filesystem::path& json_file)
{
	std::ifstream file(json_file);
	if (!file)
	{
		LOGERROR("[MappingGenerator] Can't open: {}", json_file.string());
		return;
	}
	std::stringstream ss;
	ss << file.rdbuf();
	file.close();

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

	Mappings mc_mappings = Mappings::Deserialize(data);
	
	FindMCClass(env, jvmti_env, mc_mappings);

	return;

	jclass mc = Minecraft::findMcClass(env);
	if (!mc)
	{
		LOGDEBUG("[MappingGenerator] Minecraft class not found");

		FindMCClass(env, jvmti_env, mc_mappings);
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

void MappingGenerator::FindMCClass(JNIEnv* env, jvmtiEnv* jvmti_env, const Mappings& mappings)
{
	jclass klass = findclass("java/lang/Class", g_env);
	if (!klass)
	{
		LOGERROR("[MappingGenerator] Can't find class Class");
		return;
	}
	jmethodID get_klass_name = g_env->GetMethodID(klass, "getName", "()Ljava/lang/String;");
	if (!get_klass_name)
	{
		LOGERROR("[MappingGenerator] Can't find getName");
		env->DeleteLocalRef(klass);
		return;
	}

	env->DeleteLocalRef(klass);

	std::vector<std::vector<uint8_t>> method_bytecodes;
	method_bytecodes.reserve(mappings.methods.size());
	for (const MethodMapping& method : mappings.methods)
		method_bytecodes.emplace_back(method.bytecodes);

	const float threshold = 0.8f;
	jint class_count = 0;
	jclass* classes;
	jvmti_env->GetLoadedClasses(&class_count, &classes);

	std::vector<std::pair<std::string, float>> possible_mc_classes{};

	for (int i = 0; i < class_count; i++)
	{
		jint methods_count = 0;
		jmethodID* methods;
		jvmti_env->GetClassMethods(classes[i], &methods_count, &methods);
		float similarity_score = 0;
		int similar_counter = 0;
		std::set<int> ignore_method_bytecodes{};

		for (int j = 0; j < methods_count; j++)
		{
			uint8_t* bytecodes;
			jint bytecode_count = 0;
			jvmti_env->GetBytecodes(methods[j], &bytecode_count, &bytecodes);

			std::vector<uint8_t> current_bytecodes(bytecodes, bytecodes + bytecode_count);

			// match 
			for (int k = 0; k < method_bytecodes.size(); k++)
			{
				if (ignore_method_bytecodes.contains(k))
					continue;

				float similarity = math::jaccard_index(current_bytecodes, method_bytecodes[k]);
				if (similarity > 0.8f)
				{
					similar_counter++;
					similarity_score += similarity;
					ignore_method_bytecodes.emplace(k);
					break;
				}
			}

			jvmti_env->Deallocate(bytecodes);
		}

		if ((float)similar_counter / (float)methods_count > 0.7f)
		{
			jstring klass_name = (jstring)env->CallObjectMethod(classes[i], get_klass_name);

			if (!klass_name)
			{
				LOGERROR("[MappingGenerator] Can't get name of possible minecraft class");
				break;
			}

			// add to possible minecraft class 
			possible_mc_classes.emplace_back(jstring2string(klass_name, env), similarity_score);
		}

		jvmti_env->Deallocate((unsigned char*)methods);
	}

	jvmti_env->Deallocate((unsigned char*)classes);

	LOGDEBUG("[MappingGenerator] Minecraft class possibilities: {}", possible_mc_classes.size());
	float max_score = 0;
	int index = -1;
	for (int i = 0; i < possible_mc_classes.size(); i++)
	{
		const auto& [name, score] = possible_mc_classes[i];
		if (score > max_score)
		{
			index = i;
			max_score = score;
		}
	}

	if (index != -1)
		LOGDEBUG("[MappingGenerator] Found at {} with score {} name: {}", index, max_score, possible_mc_classes[index].first);
	else
		LOGDEBUG("[MappingGenerator] Couldn't find minecraft class");
}

}