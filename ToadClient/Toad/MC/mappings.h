#pragma once

namespace toadll::mappings
{
	inline std::unordered_map<mapping, const char*> methodnames;
	inline std::unordered_map<mapping, const char*> methodsigs;

	inline const char* findName(const mapping& name)
	{
		return methodnames.find(name)->second;
	}

	inline const char* findSig(const mapping& name)
	{
		return methodsigs.find(name)->second;
	}

	inline void init_map(JNIEnv* env, const jclass mcclass, const minecraft_client& client)
	{
		if (client == minecraft_client::Vanilla)
		{

		}
		else if (client == minecraft_client::Forge)
		{
			
		}
		else if (client == minecraft_client::Lunar)
		{
			const auto getsig = [&](mapping map, const char* name) -> bool
			{
				for (int i = 0; i < jvmfunc::oJVM_GetClassMethodsCount(env, mcclass); i++)
				{
					if (std::string(jvmfunc::oJVM_GetMethodIxNameUTF(env, mcclass, i)) == name)
					{
						//std::cout << name << " = " << std::string(jvmfunc::oJVM_GetMethodIxNameUTF(env, mcclass, i)) << " sig: " << jvmfunc::oJVM_GetMethodIxSignatureUTF(env, mcclass, i) << std::endl;
						methodsigs.insert({ map, jvmfunc::oJVM_GetMethodIxSignatureUTF(env, mcclass, i)});
						return true;
					}
				}
				return false;
			};

			methodnames.insert({ mapping::getMinecraft, "getMinecraft" });
			methodsigs.insert({ mapping::getMinecraft, "()Lnet/minecraft/client/Minecraft;" });
			
			methodnames.insert({ mapping::getWorld, "bridge$getWorld" });
			if (!getsig(mapping::getWorld, "bridge$getWorld"))
				methodsigs.insert({ mapping::getWorld, "()Lcom/moonsworth/lunar/IRRRCCICICRRRCRRRCOCOCIHI/HRRCROCRCIIHIOORRIIORRHCC/HRRCROCRCIIHIOORRIIORRHCC/HORIRCRCHHRHIORIHRRRIHIIH;" });

			methodnames.insert({ mapping::getPlayer, "bridge$getPlayer" });
			if (!getsig(mapping::getPlayer, "bridge$getPlayer"))
				methodsigs.insert({ mapping::getPlayer, "()Lcom/moonsworth/lunar/IRRRCCICICRRRCRRRCOCOCIHI/HRRCROCRCIIHIOORRIIORRHCC/CCCHHICHCROHROCICOHCHHCOI/IRCOHCCIHIHRRRRRIIRHCRIHR;" });

			methodnames.insert({ mapping::getPlayerEntities, "bridge$getPlayerEntities" });
			methodsigs.insert({ mapping::getPlayerEntities, "()Ljava/util/List;" });

			methodnames.insert({ mapping::getPos, "getPositionVector" });
			methodsigs.insert({ mapping::getPos, "()Lnet/minecraft/util/Vec3;" });

			methodnames.insert({ mapping::getRotationYaw, "bridge$getRotationYaw" });
			methodnames.insert({ mapping::getRotationPitch, "bridge$getRotationPitch" });
			methodsigs.insert({ mapping::getRotationYaw, "()D" });
			methodsigs.insert({ mapping::getRotationPitch, "()D" });

			methodnames.insert({ mapping::getName, "getName" });
			methodsigs.insert({ mapping::getName, "()Ljava/lang/String;" });

			methodnames.insert({ mapping::getMotionX, "bridge$getMotionX" });
			methodnames.insert({ mapping::getMotionY, "bridge$getMotionY" });
			methodnames.insert({ mapping::getMotionZ, "bridge$getMotionZ" });
			methodsigs.insert({ mapping::getMotionX, "()D" });
			methodsigs.insert({ mapping::getMotionY, "()D" });
			methodsigs.insert({ mapping::getMotionZ, "()D" });

			methodnames.insert({ mapping::setRotationYaw, "bridge$setRotationPitch" });
			methodsigs.insert({ mapping::setRotationYaw, "(D)V" });
			methodnames.insert({ mapping::setRotationPitch, "bridge$setRotationYaw" });
			methodsigs.insert({ mapping::setRotationPitch, "(D)V" });
			methodnames.insert({ mapping::setRotation, "setRotation"});
			methodsigs.insert({ mapping::setRotation, "(FF)V"});

			// TODO: test this and add for scaffold arg = Vec3i
			// args size: 2
			// type: Vec3i
			// return type: boolean
			methodnames.insert({ mapping::isAirBlock, "isAirBlock" });
			methodsigs.insert({ mapping::isAirBlock, "(Lnet/minecraft/util/BlockPos;)Z" });

			// Vec3 class
			methodnames.insert({ mapping::Vec3X, "bridge$xCoord" });
			methodsigs.insert({ mapping::Vec3X, "()D" });
			methodnames.insert({ mapping::Vec3Y, "bridge$yCoord" });
			methodsigs.insert({ mapping::Vec3Y, "()D" });
			methodnames.insert({ mapping::Vec3Z, "bridge$zCoord" });
			methodsigs.insert({ mapping::Vec3Z, "()D" });
		}
	}
}
