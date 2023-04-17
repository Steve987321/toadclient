#pragma once
#include "Toad/Types.h"

namespace toadll::mappings
{
	const char* findName(mapping name);
	const char* findSig(mapping name);

	const char* findNameField(mappingFields name);
	const char* findSigField(mappingFields name);

	void init_map(JNIEnv* env, jclass mcclass, jclass eclass, minecraft_client client);
}
