#pragma once
#include "Toad/Types.h"

namespace toadll::mappings
{
	inline std::unordered_map<mapping, const char*> methodnames;
	inline std::unordered_map<mapping, const char*> methodsigs;

	inline std::unordered_map<mapping, const char*> fieldnames;
	inline std::unordered_map<mapping, const char*> fieldsigs;

	inline const char* findName(const mapping& name)
	{
		return methodnames.find(name)->second;
	}

	inline const char* findSig(const mapping& name)
	{
		return methodsigs.find(name)->second;
	}

	inline const char* findNameField(const mapping& name)
	{
		return fieldnames.find(name)->second;
	}

	inline const char* findSigField(const mapping& name)
	{
		return fieldsigs.find(name)->second;
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
			// use only on the minecraft class for now
			const auto getsig = [&](mapping map, const char* name, const jclass klass) -> bool
			{
				for (int i = 0; i < jvmfunc::oJVM_GetClassMethodsCount(env, klass); i++)
				{
					if (std::string(jvmfunc::oJVM_GetMethodIxNameUTF(env, klass, i)) == name)
					{
						//std::cout << name << " = " << std::string(jvmfunc::oJVM_GetMethodIxNameUTF(env, mcclass, i)) << " sig: " << jvmfunc::oJVM_GetMethodIxSignatureUTF(env, mcclass, i) << std::endl;
						methodsigs.insert({ map, jvmfunc::oJVM_GetMethodIxSignatureUTF(env, klass, i)});
						return true;
					}
				}
				return false;
			};

			methodnames.insert({ mapping::getMinecraft, "getMinecraft" });
			methodsigs.insert({ mapping::getMinecraft, "()Lnet/minecraft/client/Minecraft;" });
			
			methodnames.insert({ mapping::getWorld, "bridge$getWorld" });
			if (!getsig(mapping::getWorld, "bridge$getWorld", mcclass))
				log_Error("can't find world"); //methodsigs.insert({ mapping::getWorld, "()Lcom/moonsworth/lunar/IRRRCCICICRRRCRRRCOCOCIHI/HRRCROCRCIIHIOORRIIORRHCC/HRRCROCRCIIHIOORRIIORRHCC/HORIRCRCHHRHIORIHRRRIHIIH;" });

			methodnames.insert({ mapping::getPlayer, "bridge$getPlayer" });
			if (!getsig(mapping::getPlayer, "bridge$getPlayer", mcclass))
				log_Error("can't find player");// methodsigs.insert({ mapping::getPlayer, "()Lcom/moonsworth/lunar/IRRRCCICICRRRCRRRCOCOCIHI/HRRCROCRCIIHIOORRIIORRHCC/CCCHHICHCROHROCICOHCHHCOI/IRCOHCCIHIHRRRRRIIRHCRIHR;" });

			methodnames.insert({ mapping::getObjectMouseOver, "bridge$getObjectMouseOver" });
			if (!getsig(mapping::getObjectMouseOver, "bridge$getObjectMouseOver", mcclass))
				log_Error("can't find getobjectmouseover");// methodsigs.insert({ mapping::getObjectMouseOver, "()Lcom/moonsworth/lunar/IRRRCCICICRRRCRRRCOCOCIHI/CHOOIIHOCOHCHIIRIOHCIOCOH/IHRRCCOCORIIROHOCCCOCHCOI;" });

			methodnames.insert({ mapping::getEntityRenderer, "bridge$getEntityRenderer" });
			if (!getsig(mapping::getEntityRenderer, "bridge$getEntityRenderer", mcclass))
				log_Error("can't find entityRenderer");// methodsigs.insert({ mapping::getEntityRenderer, "()Lcom/moonsworth/lunar/IHORCOOHCIIHOHOOIHHRRHOCH/ORCIIICOHRRHCRCRRIRCCRIRR/IOHIIHOIORCROROCCHIHRCCHI/RHCOOOOHOIOCIHROHHCROHIOC/OOOCHCRHOCOCROIOOCHIRIOOR;" });

			// WORLD
			methodnames.insert({ mapping::getPlayerEntities, "bridge$getPlayerEntities" });
			methodsigs.insert({ mapping::getPlayerEntities, "()Ljava/util/List;" });

			methodnames.insert({ mapping::getRenderManager, "getRenderManager" });
			methodsigs.insert({ mapping::getRenderManager, "()Lnet/minecraft/client/renderer/entity/RenderManager;" });

			methodnames.insert({ mapping::isAirBlock, "isAirBlock" });
			methodsigs.insert({ mapping::isAirBlock, "(Lnet/minecraft/util/BlockPos;)Z" });

			// Player
			auto mcobj = env->CallStaticObjectMethod(mcclass, get_static_mid(mcclass, mapping::getMinecraft));
			auto player = env->CallObjectMethod(mcobj, get_mid(mcobj, mapping::getPlayer));
			auto playerclass = env->GetObjectClass(player);

			methodnames.insert({ mapping::getOpenContainer, "bridge$getOpenContainer" });
			if (!getsig(mapping::getOpenContainer, "bridge$getOpenContainer", playerclass))
				log_Error("can't find getinventory"); // methodsigs.insert(mapping::getOpenContainer, "bridge$getOpenContainer");

			env->DeleteLocalRef(mcobj);
			env->DeleteLocalRef(player);
			env->DeleteLocalRef(playerclass);

			// ENTITY

			fieldnames.insert({ mapping::rotationYawField, "rotationYaw" });
			fieldsigs.insert({ mapping::rotationYawField, "F" });

			fieldnames.insert({ mapping::rotationPitchField, "rotationPitch" });
			fieldsigs.insert({ mapping::rotationPitchField, "F" });

			fieldnames.insert({ mapping::prevRotationYawField, "prevRotationYaw" });
			fieldsigs.insert({ mapping::prevRotationYawField, "F" });

			fieldnames.insert({ mapping::prevRotationPitchField, "prevRotationPitch" });
			fieldsigs.insert({ mapping::prevRotationPitchField, "F" });


			methodnames.insert({ mapping::getPos, "getPositionVector" });
			methodsigs.insert({ mapping::getPos, "()Lnet/minecraft/util/Vec3;" });

			methodnames.insert({ mapping::getBlockPosition, "getPosition" });
			methodsigs.insert({ mapping::getBlockPosition, "()Lnet/minecraft/util/BlockPos;" });

			methodnames.insert({ mapping::getInventory, "getInventory" });
			methodsigs.insert({ mapping::getInventory, "()[Lnet/minecraft/item/ItemStack;" });

			methodnames.insert({ mapping::getBlockPos, "getBlockPos" });
			methodsigs.insert({ mapping::getBlockPos, "()Lnet/minecraft/util/BlockPos;" });

			methodnames.insert({ mapping::getRotationYaw, "bridge$getRotationYaw" });
			methodnames.insert({ mapping::getRotationPitch, "bridge$getRotationPitch" });
			methodsigs.insert({ mapping::getRotationYaw, "()D" });
			methodsigs.insert({ mapping::getRotationPitch, "()D" });

			/*methodnames.insert({ mapping::setSneaking, "setSneaking" });
			methodsigs.insert({ mapping::setSneaking, "(Z)V" });*/

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

			// .EntityBaseLiving
			methodnames.insert({ mapping::getHeldItem, "getHeldItem" });
			methodsigs.insert({ mapping::getHeldItem, "()Lnet/minecraft/item/ItemStack;" });

			// rendermanager
			methodnames.insert({ mapping::getRenderPosX, "getRenderPosX" });
			methodnames.insert({ mapping::getRenderPosY, "getRenderPosY" });
			methodnames.insert({ mapping::getRenderPosZ, "getRenderPosZ" });
			methodsigs.insert({ mapping::getRenderPosX, "()D" });
			methodsigs.insert({ mapping::getRenderPosY, "()D" });
			methodsigs.insert({ mapping::getRenderPosZ, "()D" });

			// ENTITY RENDERER
			methodnames.insert({ mapping::enableLightmap, "enableLightmap" });
			methodsigs.insert({ mapping::enableLightmap, "()V" });
			methodnames.insert({ mapping::disableLightmap, "disableLightmap" });
			methodsigs.insert({ mapping::disableLightmap, "()V" });

			// Vec3 class
			methodnames.insert({ mapping::Vec3X, "bridge$xCoord" });
			methodsigs.insert({ mapping::Vec3X, "()D" });
			methodnames.insert({ mapping::Vec3Y, "bridge$yCoord" });
			methodsigs.insert({ mapping::Vec3Y, "()D" });
			methodnames.insert({ mapping::Vec3Z, "bridge$zCoord" });
			methodsigs.insert({ mapping::Vec3Z, "()D" });

			// Vec3I class
			methodnames.insert({ mapping::Vec3IX, "getX" });
			methodsigs.insert({ mapping::Vec3IX, "()I" });
			methodnames.insert({ mapping::Vec3IY, "getY" });
			methodsigs.insert({ mapping::Vec3IY, "()I" });
			methodnames.insert({ mapping::Vec3IZ, "getZ" });
			methodsigs.insert({ mapping::Vec3IZ, "()I" });
		}
	}
}
