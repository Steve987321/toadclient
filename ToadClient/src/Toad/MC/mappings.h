#pragma once
#include "Toad/Types.h"

// TODO: rewrite mapping handling

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

	bool getsig(mapping map, const char* name, const jclass klass, JNIEnv* env);

	const char* findName(mapping name);
	const char* findSig(mapping name);

	const char* findNameField(mappingFields name);
	const char* findSigField(mappingFields name);

	void init_map(JNIEnv* env, jclass mcclass, jclass eclass, toad::MC_CLIENT client);
}
