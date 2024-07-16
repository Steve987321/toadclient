#pragma once

namespace toadll
{

// generate mapping patterns based on a minecraft version where there is hardcoded mappings already correct
// Use after intializing mappings 
class MappingGenerator
{
public:
	// create a mapping file (no wildcards or antyhing else)
	static void Generate(JNIEnv* jni_env, jvmtiEnv* jvmti_env);

	// use a different client with known mappings and check differences between them, will add wildcards 
	static void UpdateFile(JNIEnv* jni_env, jvmtiEnv* jvmti_env, const std::filesystem::path& json_file);

	// apply mappings from file
	static void GetMappingsFromFile(JNIEnv* jni_env, jvmtiEnv* jvmti_env, const std::filesystem::path& json_file);

private:

};

}
