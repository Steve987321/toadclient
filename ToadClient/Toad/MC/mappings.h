#pragma once
#include "Toad/Types.h"

namespace toadll::mappings
{
	struct MCMap
	{
		const char* name;
		const char* sig;
	};

	inline std::unordered_map<mapping, MCMap*> methods;
	inline std::unordered_map<mapping, const char*> methodnames;
	inline std::unordered_map<mapping, const char*> methodsigs;

	inline std::unordered_map<mappingFields, const char*> fieldnames;
	inline std::unordered_map<mappingFields, const char*> fieldsigs;

	inline bool getsig(mapping map, const char* name, const jclass klass, JNIEnv* env)
	{
		for (int i = 0; i < jvmfunc::oJVM_GetClassMethodsCount(env, klass); i++)
		{
			if (std::string(jvmfunc::oJVM_GetMethodIxNameUTF(env, klass, i)) == name)
			{
				//std::cout << name << " = " << std::string(jvmfunc::oJVM_GetMethodIxNameUTF(env, mcclass, i)) << " sig: " << jvmfunc::oJVM_GetMethodIxSignatureUTF(env, mcclass, i) << std::endl;
				methodsigs.insert({ map, jvmfunc::oJVM_GetMethodIxSignatureUTF(env, klass, i) });
				return true;
			}
		}
		return false;
	}

	const char* findName(mapping name);
	const char* findSig(mapping name);

	const char* findNameField(mappingFields name);
	const char* findSigField(mappingFields name);

	void init_map(JNIEnv* env, jclass mcclass, jclass eclass, toad::minecraft_client client);
}
