#pragma once

#include "Toad/types.h"

namespace toadll::mappings
{
	struct MCMap
	{
		MCMap(std::string_view name, std::string_view signature) 
			: name(name), sig(signature)
		{
		}

		MCMap() = default;

		std::string name;
		std::string sig;
	};

	inline std::unordered_map<mapping, MCMap> methods;
	inline std::unordered_map<mappingFields, MCMap> fields;

	bool getsig(mapping map, std::string_view name, const jclass klass, JNIEnv* env);

	void init_map(JNIEnv* env, jclass mcclass, jclass eclass, toad::MC_CLIENT client);
}
