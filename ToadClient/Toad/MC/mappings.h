#pragma once
#include "Toad/Types.h"

namespace toadll::mappings
{
	const char* findName(mapping name);
	const char* findSig(mapping name);

	const char* findNameField(mapping name);
	const char* findSigField(mapping name);

	void init_map(JNIEnv* env, jclass mcclass, minecraft_client client);
}
