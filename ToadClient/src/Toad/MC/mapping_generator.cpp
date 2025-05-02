#include "pch.h"
#include "Toad/toad.h"
#include "mappings.h"
#include "mapping_generator.h"
#include "mcutils.h"
#include "nlohmann/json.hpp"

namespace toad
{

using json = nlohmann::json;

static Mappings GetMappingsForClass(JNIEnv* env, jvmtiEnv* jvmti_env, jclass klass, int& class_index)
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

		const auto& field_mappings = mappings::fields;
		mappingFields mapping_field = mappingFields::NONE;
		for (auto& [field_mapping, field_name] : field_mappings)
		{
			if (strcmp(name, field_name.name.c_str()) == 0)
			{
				mapping_field = field_mapping;
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

		const auto& method_mappings = mappings::methods;
		mapping mapping_method = mapping::NONE;
		for (auto& [method_mapping, method_name] : method_mappings)
		{
			if (strcmp(name, method_name.name.c_str()) == 0)
			{
				mapping_method = method_mapping;
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
		LOGDEBUG("[MappingGenerator] GetLoadedClasses returned error {}", (int)jvmti_err);
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

static void InitMappingsForClass(JNIEnv* env, jvmtiEnv* jvmti_env, jclass klass, const Mappings& mappings)
{
	jint klass_methods_count;
	jmethodID* methods = nullptr;
	jvmtiError err = jvmti_env->GetClassMethods(klass, &klass_methods_count, &methods);

	// save score of mapped methods (so we can check later for better ones)
	std::unordered_map<mapping, int> mapping_methods_scored;
	std::unordered_map<mappingFields, int> mapping_fields_scored;

	if (err == JVMTI_ERROR_NONE)
	{
		// match methods with minecraft methods
		for (int i = 0; i < klass_methods_count; i++)
		{
			jmethodID method = methods[i];
			char* name;
			char* sig;
			char* generic;
			err = jvmti_env->GetMethodName(method, &name, &sig, &generic);
			if (err != JVMTI_ERROR_NONE)
				continue;

			uint8_t* bytecodes;
			jint bytecode_count = 0;
			err = jvmti_env->GetBytecodes(method, &bytecode_count, &bytecodes);
			if (err != JVMTI_ERROR_NONE)
				continue;

			std::vector<uint8_t> current_bytecodes(bytecodes, bytecodes + bytecode_count);
			mapping max;
			max = mapping::NONE;
			float max_score = 0;

			for (const MethodMapping& m : mappings.methods)
			{
				if (m.method == mapping::NONE)
					continue;

				float similarity = math::jaccard_index(m.bytecodes, current_bytecodes);
				if (similarity > max_score)
				{
					max_score = similarity;
					max = m.method;
				}
			}

			if (max != mapping::NONE)
			{
				auto it = mapping_methods_scored.find(max);
				if (it != mapping_methods_scored.end())
				{
					if (max_score > it->second)
					{
						mappings::methods[max] = { name, sig };
						it->second = max_score;
					}
				}
				else
				{
					mappings::methods[max] = { name, sig };
					mapping_methods_scored.emplace(max, max_score);
				}
			}
		}

	}
	else
		LOGERROR("[MappingGenerator] Failed to get mappings for class in InitMappingsForClass");

	jint klass_fields_count;
	jfieldID* fields = nullptr;
	err = jvmti_env->GetClassFields(klass, &klass_fields_count, &fields);

	if (err == JVMTI_ERROR_NONE)
	{
		for (int i = 0; i < klass_fields_count; i++)
		{
			jfieldID field = fields[i];
			char* name;
			char* sig;
			char* generic;
			jint modifiers = -1;
			err = jvmti_env->GetFieldName(klass, field, &name, &sig, &generic);
			jvmti_env->GetFieldModifiers(klass, field, &modifiers);
			if (err != JVMTI_ERROR_NONE)
				continue;

			mappingFields max;
			max = mappingFields::NONE;
			// the lower the better the score 
			int min_score = std::numeric_limits<int>::max();

			for (const MappingField& f : mappings.fields)
			{
				if (f.field == mappingFields::NONE)
					continue;

				int score = 0;
				score += abs(f.index - i);
				if (f.modifiers != modifiers)
					score += 1;

				//LOGDEBUG("matching {} {}, score: {}", f.name, name, score);

				if (score < min_score)
				{
					min_score = score;
					max = f.field;
				}
			}

			if (max != mappingFields::NONE)
			{
				auto it = mapping_fields_scored.find(max);
				if (it != mapping_fields_scored.end())
				{
					if (min_score < it->second)
					{
						mappings::fields[max] = { name, sig };
						it->second = min_score;
					}
				}
				else
				{
					mappings::fields[max] = { name, sig };
					mapping_fields_scored.emplace(max, min_score);
				}
			}
		}
	}

	if (methods)
		jvmti_env->Deallocate((unsigned char*)methods);
	if (fields)
		jvmti_env->Deallocate((unsigned char*)fields);
}

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

	GetJsonElement(methods_data, data, "methods");
	GetJsonElement(fields_data, data, "fields");

	for (size_t i = 0; i < methods_data.size(); i++)
	{
		MethodMapping method;
		json method_data = methods_data[i];
		GetJsonElement(method.method, method_data, "method");
		GetJsonElement(method.name, method_data, "name");
		GetJsonElement(method.signature, method_data, "signature");
		GetJsonElement(method.modifiers, method_data, "modifiers");
		GetJsonElement(method.bytecodes, method_data, "bytecodes");
		GetJsonElement(method.index, method_data, "index");

		mappings.methods.emplace_back(method);
	}

	for (size_t i = 0; i < fields_data.size(); i++)
	{
		MappingField field;
		json field_data = fields_data[i];
		GetJsonElement(field.field, field_data, "field");
		GetJsonElement(field.name, field_data, "name");
		GetJsonElement(field.signature, field_data, "signature");
		GetJsonElement(field.modifiers, field_data, "modifiers");
		GetJsonElement(field.index, field_data, "index");
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

	const auto serialize_klass = [&env, &jvmti_env](json& data, std::string_view klass_name, std::string_view json_name)
		{
			// get info 
			jclass klass = findclass(klass_name.data(), env);
			int index = -1;
			Mappings mappings = GetMappingsForClass(env, jvmti_env, klass, index);
			env->DeleteLocalRef(klass);

			// store in json 
			data[json_name]["mappings"] = mappings.Serialize();
			data[json_name]["class_index"] = index;
		};

	json data;
	serialize_klass(data, "net.minecraft.client.Minecraft", "mc");
	serialize_klass(data, "net.minecraft.client.entity.EntityPlayerSP", "player");
	serialize_klass(data, "net.minecraft.util.MovingObjectPosition", "mop");
	serialize_klass(data, "net.minecraft.world.World", "world");
	serialize_klass(data, "net.minecraft.entity.EntityLivingBase", "elb");
	serialize_klass(data, "net.minecraft.entity.Entity", "entity");
	serialize_klass(data, "net.minecraft.client.renderer.ActiveRenderInfo", "ari");
	serialize_klass(data, "net.minecraft.util.Vec3i", "vec3i");
	serialize_klass(data, "net.minecraft.util.Vec3", "vec3");
	serialize_klass(data, "net.minecraft.util.BlockPos", "blockpos");
	serialize_klass(data, "net.minecraft.entity.player.EntityPlayer", "entityplayer");
	serialize_klass(data, "net.minecraft.util.Timer", "timer");
	serialize_klass(data, "net.minecraft.client.gui.inventory.GuiChest", "guichest");
	serialize_klass(data, "net.minecraft.item.ItemStack", "itemstack");
	serialize_klass(data, "net.minecraft.block.Block", "block");
	serialize_klass(data, "net.minecraft.block.state.BlockState", "blockstate");
	serialize_klass(data, "net.minecraft.inventory.IInventory", "iinventory");

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
//	if (!get_json_element(mc_data, data, "mc"))
//	{
//		LOGERROR("[MappingGenerator] No mc found in file: {}", json_file.string());
//		return;
//	}
//	if (!get_json_element(mappings_data, mc_data, "mappings"))
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
	catch (const json::parse_error& e)
	{
		LOGERROR("[MappingGenerator] Json parse error: {}", e.what());
		return;
	}

	std::vector<Mappings> mappings;

	// json keys 
	std::string_view mappings_key_names[] =
	{
		"mc",
		"player",
		"mop",
		"world",
		"elb",
		"entity",
		"ari",
		"vec3i",
		"vec3",
		"blockpos",
		"entityplayer",
		"timer",
		"guichest",
		"itemstack",
		"block",
		"blockstate",
		"iinventory",
	};
	
	for (const std::string_view& key : mappings_key_names)
	{
		json klass_data;
		if (GetJsonElement(klass_data, data, key))
			mappings.emplace_back(Mappings::Deserialize(klass_data["mappings"]));
	}
	
	std::vector<FoundMappingKlassName> found_mappings_klass_names = FindClassTypes(env, jvmti_env, mappings);

	int minecraft_klass_index = 0;

	for (size_t i = 0; i < found_mappings_klass_names.size(); i++)
	{
		const auto& [klass_mapping, klass_name] = found_mappings_klass_names[i];

		if (i == minecraft_klass_index)
			Minecraft::unsupported_mc_class_name = klass_name;

		jclass klass = findclass(klass_name.c_str(), env);
		if (!klass)
		{
			LOGERROR("[MappingGenerator] Class not found '{}'", klass_name);
			continue;
		}

		InitMappingsForClass(env, jvmti_env, klass, klass_mapping);
		env->DeleteLocalRef(klass);
	}

	std::set<int> mappings_methods_initialized{};
	std::set<int> mappings_fields_initialized{};

	for (const auto& [mapping, map] : mappings::methods)
	{
		LOGDEBUG("{} ({}, {})", (int)mapping, map.name, map.sig);
		mappings_methods_initialized.emplace((int)mapping);
	}	
	
	for (const auto& [mapping, map] : mappings::fields)
	{
		LOGDEBUG("{} ({}, {})", (int)mapping, map.name, map.sig);
		mappings_fields_initialized.emplace((int)mapping);
	}

	// print missing 
	LOGDEBUG("[MappingGenerator] Missing {} methods", (int)mapping::COUNT - mappings_methods_initialized.size() - 1);
	LOGDEBUG("[MappingGenerator] Missing {} fields", (int)mappingFields::COUNT - mappings_fields_initialized.size() - 1);
	for (int i = 1; i < (int)mappingFields::COUNT - 1; i++)
	{
		if (mappings_fields_initialized.contains(i))
			continue;

		LOGDEBUG("[MappingGenerator] Missing field for {}", i);
	}	
	for (int i = 1; i < (int)mapping::COUNT - 1; i++)
	{
		if (mappings_methods_initialized.contains(i))
			continue;

		LOGDEBUG("[MappingGenerator] Missing method for {}", i);
	}
}

std::vector<FoundMappingKlassName> MappingGenerator::FindClassTypes(JNIEnv* env, jvmtiEnv* jvmti_env, const std::vector<Mappings>& klass_mappings)
{
	std::vector<FoundMappingKlassName> res{};

	struct KlassAndScore
	{
		int index;
		std::string klass_name;
		float score;
	};

	// bytecodes and other method info per mapping
	std::unordered_map<size_t, std::vector<std::vector<uint8_t>>> mappings_similarity_check{};

	for (size_t i = 0; i < klass_mappings.size(); i++) 
	{
		for (MethodMapping method : klass_mappings[i].methods)
		{
			method.bytecodes.emplace_back(method.modifiers);
			method.bytecodes.emplace_back(method.index);
			mappings_similarity_check[i].emplace_back(std::move(method.bytecodes));
		}
	}

	for (const auto& mapping : mappings_similarity_check)
		LOGDEBUG("[MappingGenerator] index: {} size: {}", mapping.first, mapping.second.size());

	jint klass_count = 0;
	jclass* klasses = nullptr;
	jvmti_env->GetLoadedClasses(&klass_count, &klasses);

	LOGDEBUG("[MappingGenerator] Loaded classes: {}", klass_count);

	// possibilities 
	std::unordered_map<size_t, std::vector<KlassAndScore>> possible_klasses{};

	// similarity score for class [klass mapping index, [klass index, score]]
	std::unordered_map<size_t, std::unordered_map<int, float>> similarity_score;

	// how many methods are similar enough
	std::unordered_map<size_t, std::unordered_map<int, int>> similar_counter;

	// should lock when writing or reading data
	std::mutex mutex;

	const auto process_for_data = [&](int start_index, int end_index)
		{
			g_jvm->AttachCurrentThread((void**)&env, nullptr);

			std::vector<uint8_t> current_method_info;

			for (int i = start_index; i < end_index; i++)
			{
				jint methods_count = 0;
				jmethodID* methods = nullptr;
				jvmtiError err = jvmti_env->GetClassMethods(klasses[i], &methods_count, &methods);

				if (err != JVMTI_ERROR_NONE || methods_count == 0)
					continue;

				// mappings indexes that can be skipped for this iteration
				std::set<size_t> ignore_mappings_index{};

				// check if we can skip some checks for certain mappings.
				for (size_t j = 0; j < klass_mappings.size(); j++)
				{
					// check if methods count is too different
					float methods_count_diff = abs(methods_count - (int)klass_mappings[j].methods.size());
					if (err != JVMTI_ERROR_NONE ||
						methods_count == 0 ||
						klass_mappings[j].methods.empty() ||
						methods_count_diff / klass_mappings[j].methods.size() > 0.55f)
					{
						ignore_mappings_index.emplace(j);
					}
				}

				// all mappings can be ignored so skip
				if (ignore_mappings_index.size() == klass_mappings.size())
					continue;

				for (int j = 0; j < methods_count; j++)
				{
					uint8_t* bytecodes = nullptr;
					jint bytecode_count = 0;

					// returns error 104 JVMTI_ERROR_NATIVE_METHOD sometimes
					err = jvmti_env->GetBytecodes(methods[j], &bytecode_count, &bytecodes);

					if (err != JVMTI_ERROR_NONE)
						continue;
					if (bytecode_count == 0)
						continue;

					current_method_info.clear();
					current_method_info.insert(current_method_info.end(), bytecodes, bytecodes + bytecode_count);

					jint mod = -1;
					jvmti_env->GetMethodModifiers(methods[j], &mod);
					current_method_info.emplace_back(mod);
					current_method_info.emplace_back(j);

					for (const auto& [index, mapping_function_info] : mappings_similarity_check)
					{
						if (ignore_mappings_index.contains(index))
							continue;

						if (mapping_function_info.empty() || current_method_info.empty())
							continue;

						// match methods 
						for (const std::vector<uint8_t>& method_info : mapping_function_info)
						{
							float method_info_count_diff = abs((int)current_method_info.size() - (int)method_info.size());
							if (method_info_count_diff / method_info.size() > 0.65f)
								continue;

							float similarity = math::jaccard_index(current_method_info, method_info);
							if (similarity > 0.65f)
							{
								mutex.lock();
								similar_counter[index][i]++;
								similarity_score[index][i] += similarity;
								mutex.unlock();
							}
						}
					}

					jvmti_env->Deallocate(bytecodes);
				}

				jvmti_env->Deallocate((unsigned char*)methods);

				//LOGDEBUG("[MappingGenerator] {} / {} = {}", similar_counter, methods_count, (float)similar_counter / ((float)methods_count + FLT_EPSILON));

				for (size_t j = 0; j < klass_mappings.size(); j++)
				{
					if (ignore_mappings_index.contains(j))
						continue;

					std::unique_lock lock(mutex);
					if ((float)similar_counter[j][i] / (float)methods_count > 0.6f)
					{
						lock.unlock();
						LOGDEBUG("[MappingGenerator] Found a possibility for {} with score {}", j, similarity_score[j][i]);
						
						mutex.lock();
						possible_klasses[j].emplace_back(i, "", similarity_score[j][i]);
						mutex.unlock();

						LOGDEBUG("[MappingGenerator] Thread progress: {}", (float)i / end_index);
					}
				}
			}

			g_jvm->DetachCurrentThread();
		};

	int nthreads = (int)std::thread::hardware_concurrency() / 2;
	LOGDEBUG("[MappingGenerator] Using {} of the available {} threads", nthreads, std::thread::hardware_concurrency());
	
	std::vector<std::thread> threads;
	threads.reserve(nthreads);

	int chunk_size = klass_count / nthreads;
	int start_index = 0;
	for (int i = 0; i < nthreads; i++)
	{
		int end_index = 0;
		if (i == nthreads - 1)
			end_index = klass_count; // account for remainder
		else
			end_index = start_index + chunk_size;

		threads.emplace_back(process_for_data, start_index, end_index);

		start_index = end_index;
	}
	
	for (std::thread& thread : threads)
	{
		if (thread.joinable())
			thread.join();
	}

	LOGDEBUG("[MappingGenerator] Found {}/{}", possible_klasses.size(), klass_mappings.size());

	g_jvm->AttachCurrentThread((void**)&env, nullptr);

	// get name method id for getting class names
	jclass klass = findclass("java/lang/Class", env);
	if (!klass)
	{
		LOGERROR("[MappingGenerator] Can't find class Class");
		return res;
	}
	jmethodID get_klass_name = env->GetMethodID(klass, "getName", "()Ljava/lang/String;");
	if (!get_klass_name)
	{
		LOGERROR("[MappingGenerator] Can't find String getName()");
		env->DeleteLocalRef(klass);
		return res;
	}
	env->DeleteLocalRef(klass);

	for (size_t i = 0; i < klass_mappings.size(); i++)
	{
		if (possible_klasses.contains(i))
		{
			// get class names 
			for (auto& klass_and_score : possible_klasses[i])
			{
				if (klass_and_score.index == -1)
					continue;

				jclass klass = klasses[klass_and_score.index];

				jstring klass_name_obj = (jstring)env->CallObjectMethod(klass, get_klass_name);
				if (!klass_name_obj)
				{
					LOGERROR("[MappingGenerator] Can't get name of possible class");
					continue;
				}

				std::string klass_name_str = jstring2string(klass_name_obj, env);
				env->DeleteLocalRef(klass_name_obj);

				klass_and_score.klass_name = std::move(klass_name_str);
			}

			LOGDEBUG("[MappingGenerator] Class possibilities for {}: {}", i, possible_klasses[i].size());
		}
		else
			LOGDEBUG("[MappingGenerator] None found for", i);
	}

	jvmti_env->Deallocate((unsigned char*)klasses);

	for (size_t i = 0; i < klass_mappings.size(); i++)
	{
		std::string klass_name;
		float max_score = 0;
		int index = -1;

		for (size_t j = 0; j < possible_klasses[i].size(); j++)
		{
			if (possible_klasses[i][j].score > max_score)
			{
				index = j;
				max_score = possible_klasses[i][j].score;
				klass_name = possible_klasses[i][j].klass_name;
			}
		}
		
		LOGDEBUG("[MappingGenerator] Result for mapping {} with name {} and score {}", i, klass_name, max_score);
		res.emplace_back(klass_mappings[i], klass_name);
	}

	return res;
}

}