#pragma once

#include "nlohmann/json.hpp"

namespace toad
{

using json = nlohmann::json;

struct MappingField
{
	mappingFields field;
	std::string name;
	std::string signature;
	int modifiers;
	int index;
};

struct MethodMapping
{
	mapping method;
	std::string name;
	std::string signature;
	std::vector<uint8_t> bytecodes;
	int modifiers;
	int index;
};

// all classes that are being used 
enum class MC_CLASS_TYPE
{
	MC,
	BLOCKPOS,
	PLAYER,
	WORLD,
	WORLDCLIENT,
};

class Mappings
{
public:
	std::vector<MappingField> fields;
	std::vector<MethodMapping> methods;

	json Serialize();
	static Mappings Deserialize(const nlohmann::json& data);
};

struct FoundMappingKlassName
{
	Mappings mappings;
	std::string found_klass_name;
};

// generate mapping patterns based on a minecraft version where there is hardcoded mappings already correct
// Use Generate after intializing mappings 
namespace MappingGenerator
{
	// create a mapping file 
	void Generate(JNIEnv* jni_env, jvmtiEnv* jvmti_env);

	// #TODO: maybe not needed because just use similarity check
	// use a different client with known mappings and check differences between them, will add wildcards 
	//void GetMappingsFromFile(JNIEnv* jni_env, jvmtiEnv* jvmti_env, const std::filesystem::path& json_file);

	void InitMappings(JNIEnv* env, jvmtiEnv* jvmti_env, const std::filesystem::path& file);

	// apply mappings from file
	//void GetMappingsFromFile(JNIEnv* jni_env, jvmtiEnv* jvmti_env, const std::filesystem::path& json_file);

	std::vector<FoundMappingKlassName> FindClassTypes(JNIEnv* env, jvmtiEnv* jvmti_env, const std::vector<Mappings>& mappings);
};

}
