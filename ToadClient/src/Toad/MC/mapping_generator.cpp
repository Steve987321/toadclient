#include "pch.h"
#include "Toad/toadll.h"
#include "mappings.h"
#include "mapping_generator.h"
#include "mcutils.h"
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

	for (size_t i = 0; i < fields.size(); i++)
	{
		json field_data;

		field_data["field"] = fields[i].field;
		field_data["modifiers"] = fields[i].modifiers;
		field_data["signature"] = fields[i].signature;
		field_data["name"] = fields[i].name;
		field_data["index"] = fields[i].index;

		fields_data[i] = field_data;
	}

	for (size_t i = 0; i < methods.size(); i++)
	{
		json method_data;
		method_data["method"] = methods[i].method;
		method_data["modifiers"] = methods[i].modifiers;
		method_data["signature"] = methods[i].signature;
		method_data["name"] = methods[i].name;
		method_data["bytecodes"] = methods[i].bytecodes;
		method_data["index"] = methods[i].index;

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

	for (size_t i = 0; i < methods_data.size(); i++)
	{
		MethodMapping method;
		json method_data = methods_data[i];
		config::get_json_element(method.method, method_data, "method");
		config::get_json_element(method.name, method_data, "name");
		config::get_json_element(method.signature, method_data, "signature");
		config::get_json_element(method.modifiers, method_data, "modifiers");
		config::get_json_element(method.bytecodes, method_data, "bytecodes");
		config::get_json_element(method.index, method_data, "index");

		mappings.methods.emplace_back(method);
	}

	for (size_t i = 0; i < fields_data.size(); i++)
	{
		MappingField field;
		json field_data = fields_data[i];
		config::get_json_element(field.field, field_data, "field");
		config::get_json_element(field.name, field_data, "name");
		config::get_json_element(field.signature, field_data, "signature");
		config::get_json_element(field.modifiers, field_data, "modifiers");
		config::get_json_element(field.index, field_data, "index");
		mappings.fields.emplace_back(field);
	}
	
	return mappings;
}

void MappingGenerator::Generate(JNIEnv* env, jvmtiEnv* jvmti_env)
{
	CHAR documents[MAX_PATH];
	HRESULT get_folder_path_res = SHGetFolderPathA(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, documents);
	if (get_folder_path_res != S_OK)
	{
		LOGERROR("[MappingGenerator] Failed to get documents folder");
		return;
	}

	jclass mc_class = Minecraft::getMcClass(env);
	jclass player_class = findclass(mappings::fields[mappingFields::thePlayerField].sig.c_str(), env);
	jclass world_client_class = findclass("net.minecraft.client.multiplayer.WorldClient", env);
	jclass world_class = findclass("net.minecraft.world.World", env);
	jclass block_pos_class = findclass("net.minecraft.util.MovingObjectPosition", env);
	int mc_class_index = -1;
	int world_class_index = -1;
	int world_client_class_index = -1;
	int player_class_index = -1;
	int block_pos_class_index = -1;
	Mappings mc_mappings = GetMappingsForClass(env, jvmti_env, mc_class, mc_class_index);
	Mappings world_client_mappings = GetMappingsForClass(env, jvmti_env, world_client_class, world_client_class_index);
	Mappings world_mappings = GetMappingsForClass(env, jvmti_env, world_class, world_class_index);
	Mappings player_mappings = GetMappingsForClass(env, jvmti_env, player_class, player_class_index);
	Mappings block_pos_mappings = GetMappingsForClass(env, jvmti_env, block_pos_class, block_pos_class_index);
	env->DeleteLocalRef(world_client_class);
	env->DeleteLocalRef(mc_class);
	env->DeleteLocalRef(player_class);
	env->DeleteLocalRef(world_class);
	env->DeleteLocalRef(block_pos_class);

	json data;
	data["mc"]["mappings"] = mc_mappings.Serialize();
	data["mc"]["class_index"] = mc_class_index; 
	data["worldclient"]["mappings"] = world_client_mappings.Serialize();
	data["worldclient"]["class_index"] = world_client_class_index;
	data["world"]["mappings"] = world_mappings.Serialize();
	data["world"]["class_index"] = world_class_index;
	data["player"]["mappings"] = player_mappings.Serialize();
	data["player"]["class_index"] = player_class_index;
	data["blockpos"]["mappings"] = block_pos_mappings.Serialize();
	data["blockpos"]["class_index"] = block_pos_class_index;
	std::ofstream file(std::filesystem::path(documents) / "mapping_gen_out.txt");
	if (!file)
	{
		LOGERROR("[MappingGenerator] Failed to create output file");
		return;
	}
	file << data << std::endl;
	file.close();
}

//void MappingGenerator::GetMappingsFromFile(JNIEnv* env, jvmtiEnv* jvmti_env, const std::filesystem::path& json_file)
//{
//	std::ifstream file(json_file);
//	if (!file)
//	{
//		LOGERROR("[MappingGenerator] Can't open: {}", json_file.string());
//		return;
//	}
//	std::stringstream ss;
//	ss << file.rdbuf();
//	file.close();
//
//	json data;
//	try
//	{
//		data = json::parse(ss.str());
//	}
//	catch (json::parse_error& e)
//	{
//		LOGERROR("[MappingGenerator] json parse error: {}", e.what());
//		return;
//	}
//
//	json mc_data;
//	json mappings_data;
//	if (!config::get_json_element(mc_data, data, "mc"))
//	{
//		LOGERROR("[MappingGenerator] No mc found in file: {}", json_file.string());
//		return;
//	}
//	if (!config::get_json_element(mappings_data, mc_data, "mappings"))
//	{
//		LOGERROR("[MappingGenerator] No mappings found in file: {}", json_file.string());
//		return;
//	}
//
//	Mappings mc_mappings = Mappings::Deserialize(mappings_data);
//
//	jclass mc = Minecraft::getMcClass(env);
//	std::string mc_class_name; 
//	if (!mc)
//	{
//		mc_class_name = FindClassTypes(env, jvmti_env, mc_mappings);
//
//		mc = findclass(mc_class_name.c_str(), env);
//
//		if (!mc)
//		{
//			LOGERROR("[MappingGenerator] Minecraft class can't be found");
//			return;
//		}
//	}
//
//	jint field_count = 0;
//	jfieldID* fields;
//
//	jvmtiError res;
//	res = jvmti_env->GetClassFields(mc, &field_count, &fields);
//	if (res != jvmtiError::JVMTI_ERROR_NONE)
//	{
//		LOGERROR("[MappingGenerator] Failed to call GetClassFields, {}", (int)res);
//		env->DeleteLocalRef(mc);
//		return;
//	}
//
//	//std::unordered_map<mapping, MCMap> methods;
//	std::unordered_map<mappingFields, MCMap> mc_fields{};
//
//	for (int i = 0; i < field_count; i++)
//	{
//		char* name;
//		char* sig;
//		char* gen;
//		if (jvmti_env->GetFieldName(mc, fields[i], &name, &sig, &gen) != JVMTI_ERROR_NONE)
//			continue;
//
//		// theMinecraft
//		if (strncmp(name, ('L' + mc_class_name + ';').c_str(), mc_class_name.size()) == 0)
//		{
//			mc_fields[mappingFields::theMcField] = MCMap{ name, sig };
//		}
//	}
//}

void MappingGenerator::InitMappings(JNIEnv* env, jvmtiEnv* jvmti_env, const std::filesystem::path& file)
{
	std::ifstream f(file);
	if (!f)
		return;

	json data; 
	try
	{
		data = json::parse(f);
	}
	catch (json::parse_error& e)
	{
		return;
	}

	json mc_data;
	json player_data;
	json world_data;
	json world_client_data;
	json block_pos_data;
	config::get_json_element(mc_data, data, "mc");
	config::get_json_element(world_client_data, data, "worldclient");
	config::get_json_element(world_data, data, "world");
	config::get_json_element(player_data, data, "player");
	config::get_json_element(block_pos_data, data, "blockpos");

	Mappings mc_mappings = Mappings::Deserialize(mc_data["mappings"]);
	Mappings player_mappings = Mappings::Deserialize(player_data["mappings"]);

	// #TODO: optimize later 
	// #TODO: sorted methods count 
	//std::queue<int> min_methods_count;
	//min_methods_count.push(0);

	std::string mc_class_name = FindClassTypes(env, jvmti_env, mc_mappings);
	Minecraft::unsupported_mc_class_name = mc_class_name;
	jclass minecraft = findclass(mc_class_name.c_str(), env);
	jint methods_count;
	jmethodID* methods = nullptr;
	jvmtiError err = jvmti_env->GetClassMethods(minecraft, &methods_count, &methods);
	
	if (err == JVMTI_ERROR_NONE)
	{
		// match methods with minecraft methods

		// get bytecodes
		for (int i = 0; i < methods_count; i++)
		{
			jmethodID method = methods[i];

			for (const auto& m : mc_mappings.methods)
			{
				if (m.method == mapping::NONE)
					continue;

				char* name;
				char* sig;
				char* generic;
				err = jvmti_env->GetMethodName(method, &name, &sig, &generic);
				if (err != JVMTI_ERROR_NONE) 
				{
					mappings::methods[m.method] = { name, sig };
					break;
				}
			}
		}
	}
}

std::string MappingGenerator::FindClassTypes(JNIEnv* env, jvmtiEnv* jvmti_env, const Mappings& mappings)
{
	// to string method id for class names
	jclass klass = findclass("java/lang/Class", g_env);
	if (!klass)
	{
		LOGERROR("[MappingGenerator] Can't find class Class");
		return "";
	}
	jmethodID get_klass_name = g_env->GetMethodID(klass, "getName", "()Ljava/lang/String;");
	if (!get_klass_name)
	{
		LOGERROR("[MappingGenerator] Can't find getName");
		env->DeleteLocalRef(klass);
		return "";
	}
	env->DeleteLocalRef(klass);

	std::vector<std::vector<uint8_t>> mapping_method_bytecodes;
	mapping_method_bytecodes.reserve(mappings.methods.size());
	for (const MethodMapping& method : mappings.methods)
		mapping_method_bytecodes.emplace_back(method.bytecodes);

	LOGDEBUG("[MappingGenerator] Mapping bytecodes size: {}", mapping_method_bytecodes.size());

	jint class_count = 0;
	jclass* classes;
	jvmti_env->GetLoadedClasses(&class_count, &classes);

	LOGDEBUG("[MappingGenerator] Classes: {}", class_count);

	std::vector<std::pair<std::string, float>> possible_classes{};

	for (int i = 0; i < class_count; i++)
	{
		// get methods
		jint methods_count = 0;
		jmethodID* methods = nullptr;
		jvmtiError err = jvmti_env->GetClassMethods(classes[i], &methods_count, &methods);

		if (err != JVMTI_ERROR_NONE)
			continue;

		if (methods_count < 10)
			continue;

		float similarity_score = 0;
		int similar_counter = 0;

		for (int j = 0; j < methods_count; j++)
		{
			uint8_t* bytecodes;
			jint bytecode_count = 0;
			err = jvmti_env->GetBytecodes(methods[j], &bytecode_count, &bytecodes);
			if (err != JVMTI_ERROR_NONE)
				continue;

			std::vector<uint8_t> current_bytecodes(bytecodes, bytecodes + bytecode_count);
			std::set<int> ignore_method_bytecodes{};

			// match 
			for (int k = 0; k < mapping_method_bytecodes.size(); k++)
			{
				if (ignore_method_bytecodes.contains(k))
					continue;

				float similarity = math::jaccard_index(current_bytecodes, mapping_method_bytecodes[k]);
				if (similarity > 0.7f)
				{
					similar_counter++;
					similarity_score += similarity;
					ignore_method_bytecodes.emplace(k);
					k = 0;
				}
			}

			jvmti_env->Deallocate(bytecodes);
		}

		if (methods)
			jvmti_env->Deallocate((unsigned char*)methods);

		//LOGDEBUG("[MappingGenerator] {} / {} = {}", similar_counter, methods_count, (float)similar_counter / ((float)methods_count + FLT_EPSILON));
		if ((float)similar_counter / ((float)methods_count + FLT_EPSILON) > 0.7f)
		{
			LOGDEBUG("[MappingGenerator] Found a possibility with score {}", similarity_score);
			jstring klass_name = (jstring)env->CallObjectMethod(classes[i], get_klass_name);

			if (!klass_name)
			{
				LOGERROR("[MappingGenerator] Can't get name of possible minecraft class");
				break;
			}

			// add to possible minecraft class 
			possible_classes.emplace_back(jstring2string(klass_name, env), similarity_score);

			env->DeleteLocalRef(klass_name);
		}
	}

	jvmti_env->Deallocate((unsigned char*)classes);

	LOGDEBUG("[MappingGenerator] Minecraft class possibilities: {}", possible_classes.size());
	float max_score = 0;
	int index = -1;
	for (int i = 0; i < possible_classes.size(); i++)
	{
		const auto& [name, score] = possible_classes[i];
		if (score > max_score)
		{
			index = i;
			max_score = score;
		}
	}

	if (index != -1)
		LOGDEBUG("[MappingGenerator] Found at {} with score {} name: {}", index, max_score, possible_classes[index].first);
	else
		LOGDEBUG("[MappingGenerator] Couldn't find minecraft class");

	return possible_classes[index].first;
}

Mappings MappingGenerator::GetMappingsForClass(JNIEnv* env, jvmtiEnv* jvmti_env, jclass klass, int& class_index)
{
	if (!klass)
	{
		LOGERROR("[MappingGenerator] klass argument is null");
		return {};
	}
	Mappings klass_mappings;
	jint field_count = 0;
	jfieldID* fields;

	jvmtiError jvmti_err;
	//JVMTICALL(jvmti_env->GetClassFields, mc, &field_count, &fields)
	jvmti_err = jvmti_env->GetClassFields(klass, &field_count, &fields);
	if (jvmti_err != jvmtiError::JVMTI_ERROR_NONE)
	{
		LOGERROR("[MappingGenerator] Failed to call GetClassFields, {}", (int)jvmti_err);
		env->DeleteLocalRef(klass);
		return klass_mappings;
	}

	LOGDEBUG("[MappingGenerator] Fields");
	for (int i = 0; i < field_count; i++)
	{
		char* name = nullptr;
		char* sig = nullptr;
		char* generic = nullptr;
		jint modifiers = 0;

		if (jvmti_err = jvmti_env->GetFieldName(klass, fields[i], &name, &sig, &generic); jvmti_err != JVMTI_ERROR_NONE)
		{
			LOGERROR("[MappingGenerator] Failed to call GetFieldName, {}", (int)jvmti_err);
			continue;
		}
		if (jvmti_err = jvmti_env->GetFieldModifiers(klass, fields[i], &modifiers); jvmti_err != JVMTI_ERROR_NONE)
		{
			LOGERROR("[MappingGenerator] Failed to call GetFieldModifiers, {}", (int)jvmti_err);
			continue;
		}

		static auto field_mappings = mappings::fields;
		mappingFields mapping_field = mappingFields::NONE;
		for (auto& [field_mapping, field_name] : field_mappings)
		{
			if (strcmp(name, field_name.name.c_str()) == 0)
			{
				mapping_field = field_mapping;
				field_mappings.erase(field_mapping);
				break;
			}
		}

		MappingField field;
		field.field = mapping_field;
		field.name = name;
		field.signature = sig;
		field.modifiers = modifiers;
		field.index = i;
		klass_mappings.fields.emplace_back(field);
	}

	LOGDEBUG("[MappingGenerator] Fields done");
	jvmti_env->Deallocate((unsigned char*)fields);

	LOGDEBUG("[MappingGenerator] Methods");

	jint method_count = 0;
	jmethodID* methods;
	jvmti_err = jvmti_env->GetClassMethods(klass, &method_count, &methods);
	if (jvmti_err != jvmtiError::JVMTI_ERROR_NONE)
	{
		LOGERROR("[MappingGenerator] Failed to call GetClassMethods, {}", (int)jvmti_err);
		env->DeleteLocalRef(klass);
		return klass_mappings;
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

		if (jvmti_err = jvmti_env->GetBytecodes(methods[i], &bytecodes_count, &bytecodes); jvmti_err != JVMTI_ERROR_NONE)
		{
			LOGERROR("[MappingGenerator] Failed to call GetBytecodes, {}", (int)jvmti_err);
			continue;
		}
		if (jvmti_err = jvmti_env->GetMethodModifiers(methods[i], &modifiers); jvmti_err != JVMTI_ERROR_NONE)
		{
			LOGERROR("[MappingGenerator] Failed to call GetMethodModifiers, {}", (int)jvmti_err);
			continue;
		}
		if (jvmti_err = jvmti_env->GetMethodName(methods[i], &name, &sig, &generic); jvmti_err != JVMTI_ERROR_NONE)
		{
			LOGERROR("[MappingGenerator] Failed to call GetMethodName, {}", (int)jvmti_err);
			continue;
		}

		bytecodes_vec.clear();
		bytecodes_vec.reserve(bytecodes_count);
		for (int j = 0; j < bytecodes_count; j++)
			bytecodes_vec.emplace_back(bytecodes[j]);

		static auto method_mappings = mappings::methods;
		mapping mapping_method = mapping::NONE;
		for (auto& [method_mapping, method_name] : method_mappings)
		{
			if (strcmp(name, method_name.name.c_str()) == 0)
			{
				mapping_method = method_mapping;
				method_mappings.erase(method_mapping);
				break;
			}
		}

		MethodMapping method;
		method.method = mapping_method;
		method.name = name;
		method.signature = sig;
		method.modifiers = modifiers;
		method.bytecodes = bytecodes_vec;
		method.index = i;
		klass_mappings.methods.emplace_back(method);
	}

	jvmti_env->Deallocate((unsigned char*)methods);
	env->DeleteLocalRef(klass);

	// get class index 

	jint class_count = 0;
	jclass* classes;
	jvmti_err = jvmti_env->GetLoadedClasses(&class_count, &classes);
	if (jvmti_err != JVMTI_ERROR_NONE)
	{
		env->DeleteLocalRef(klass);
		return klass_mappings;
	}

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

	return klass_mappings;
}

}