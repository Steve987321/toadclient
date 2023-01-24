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

	inline void init_map(/*JNIEnv* env,*/ const minecraft_client& client)
	{
		
		if (client == minecraft_client::Vanilla)
		{

		}
		else if (client == minecraft_client::Forge)
		{
			
		}
		else if (client == minecraft_client::Lunar)
		{
			methodnames.insert({ mapping::getMinecraft, "getMinecraft" });
			methodsigs.insert({ mapping::getMinecraft, "()Lnet/minecraft/client/Minecraft;" });

			
			methodnames.insert({ mapping::getWorld, "bridge$getWorld" });
			/*for (int i = 0; i < jvmfunc::oJVM_GetClassMethodsCount(env, findclass("net.")); i++)
			{
			}*/

			methodsigs.insert({ mapping::getWorld, "()Lcom/moonsworth/lunar/IRRRCCICICRRRCRRRCOCOCIHI/HRRCROCRCIIHIOORRIIORRHCC/HRRCROCRCIIHIOORRIIORRHCC/HORIRCRCHHRHIORIHRRRIHIIH;" });

			methodnames.insert({ mapping::getPlayer, "bridge$getPlayer" });
			methodsigs.insert({ mapping::getPlayer, "()Lcom/moonsworth/lunar/OCIROORRIRIOHIIIRRIORCIHI/RCHHIHHRCCIHIRCCCRHIOCCHR/OCRHRIOCIICCRHCHHHOHHCROC/OIOORRORCRHOOCICORRCHOHRC;" });

			methodnames.insert({ mapping::getPlayerEntities, "bridge$getPlayerEntities" });
			methodsigs.insert({ mapping::getPlayerEntities, "()Ljava/util/List;" });

			methodnames.insert({ mapping::getPos, "getPositionVector" });
			methodsigs.insert({ mapping::getPos, "()Lnet/minecraft/util/Vec3;" });

			methodnames.insert({ mapping::getRotationYaw, "bridge$getRotationYaw" });
			methodnames.insert({ mapping::getRotationPitch, "bridge$getRotationPitch" });
			methodsigs.insert({ mapping::getRotationYaw, "()D" });
			methodsigs.insert({ mapping::getRotationPitch, "()D" });

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

			// TODO: test this and add for scaffold arg = Vec3i 
			//name: isAirBlock sig: (Lnet/minecraft/util/BlockPos;)Z args size: 2

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
