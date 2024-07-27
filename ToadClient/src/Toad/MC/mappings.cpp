#include "pch.h"
#include "Toad/toadll.h"
#include "mappings.h"

namespace toadll::mappings
{
	void init_map(JNIEnv* env, jclass mcclass, jclass entity_class, toad::MC_CLIENT client)
	{
		if (client == toad::MC_CLIENT::Lunar_189)
		{
			fields[mappingFields::theMcField] = { "theMinecraft", "Lnet/minecraft/client/Minecraft;" };
			fields[mappingFields::theWorldField] = { "theWorld", "Lnet/minecraft/client/multiplayer/WorldClient;" };
			
			fields[mappingFields::objMouseOver] = { "objectMouseOver", "Lnet/minecraft/util/MovingObjectPosition;" };
			fields[mappingFields::timer] = { "timer", "Lnet/minecraft/util/Timer;" };

			methods[mapping::getBlockState] = { "getBlockState", "(Lnet/minecraft/util/BlockPos;)Lnet/minecraft/block/state/IBlockState;" };
			methods[mapping::getBlock] = { "getBlock", "()Lnet/minecraft/block/Block;" };

		/*	methods[mapping::getPlayer] = { "bridge$getPlayer", "SIGNATURE NOT FOUND" };
			if (!getsig(mapping::getPlayer, "bridge$getPlayer", mcclass, env))
				LOGERROR("[mapping] Can't find getPlayer");*/

			fields[mappingFields::thePlayerField] = { "thePlayer", "Lnet/minecraft/client/entity/EntityPlayerSP;"};
			fields[mappingFields::gameSettings] = { "gameSettings", "Lnet/minecraft/client/settings/GameSettings;" };
			
			fields[mappingFields::currentScreenField] = { "currentScreen", "Lnet/minecraft/client/gui/GuiScreen;" };
			fields[mappingFields::leftClickCounterField] = { "leftClickCounter" , "I"};

			methods[mapping::getEntityHit] = {"bridge$getEntityHit", "SIGNATURE NOT FOUND" };
			// methodsig is in minecraft.cpp

			// General
			methods[mapping::toString] = { "toString", "()Ljava/lang/String;" };
			methods[mapping::getDisplayName] = { "getDisplayName", "()Ljava/lang/String;" };

			// WORLD
			methods[mapping::getPlayerEntities] = { "bridge$getPlayerEntities", "()Ljava/util/List;" };
			methods[mapping::getRenderManager] = { "getRenderManager", "()Lnet/minecraft/client/renderer/entity/RenderManager;" };
			methods[mapping::isAirBlock] = { "isAirBlock", "(Lnet/minecraft/util/BlockPos;)Z" };
			methods[mapping::rayTraceBlocks] = { "rayTraceBlocks", "(Lnet/minecraft/util/Vec3;Lnet/minecraft/util/Vec3;Z)Lnet/minecraft/util/MovingObjectPosition;" };
			methods[mapping::getBlockFromBlockState] = { "getBlock", "()Lnet/minecraft/block/Block;" };
			methods[mapping::getIdFromBlockStatic] = { "getIdFromBlock", "(Lnet/minecraft/block/Block;)I" };

			fields[mappingFields::playerEntitiesField] = { "playerEntities", "Ljava/util/List;" };

			// .GameSettings
			methods[mapping::setGamma] = { "bridge$setGamma", "(F)V" };

			fields[mappingFields::fovField] = { "fovSetting", "F" };

			// ENTITY
			fields[mappingFields::rotationYawField] = { "rotationYaw", "F" };
			fields[mappingFields::rotationPitchField] = { "rotationPitch", "F" };
			fields[mappingFields::prevRotationYawField] = { "prevRotationYaw", "F" };
			fields[mappingFields::prevRotationPitchField] = { "prevRotationPitch", "F" };

			fields[mappingFields::motionXField] = { "motionX", "D" };
			fields[mappingFields::motionYField] = {"motionY", "D"};
			fields[mappingFields::motionZField] = {"motionZ", "D"};

			fields[mappingFields::EntityPosX] = { "posX", "D" };
			fields[mappingFields::EntityPosY] = { "posY", "D" };
			fields[mappingFields::EntityPosZ] = { "posZ", "D" };

			fields[mappingFields::hurtTimeI] = { "hurtTime" , "I" };

			fields[mappingFields::lastTickPosXField] = { "lastTickPosX", "D" };
			fields[mappingFields::lastTickPosYField] = {"lastTickPosY", "D"};
			fields[mappingFields::lastTickPosZField] = {"lastTickPosZ", "D"};

			methods[mapping::getPos] = {"getPositionVector", "()Lnet/minecraft/util/Vec3;"};
			methods[mapping::getBlockPosition] = {"getPosition", "()Lnet/minecraft/util/BlockPos;" };
			methods[mapping::isSneaking] = {"isSneaking", "()Z" };

			jclass movingblockklass = findclass("net.minecraft.util.MovingObjectPosition", env);
			methods[mapping::getBlockPositionFromMovingBlock] = {"bridge$getBlockPosition", "SIGNATURE NOT FOUND" };
			if (!getsig(mapping::getBlockPositionFromMovingBlock, "bridge$getBlockPosition", movingblockklass, env))
				LOGERROR("can't find bridge$getBlockPosition from moving block class");

			env->DeleteLocalRef(movingblockklass);

			fields[mappingFields::inventoryField] = { "inventory", "Lnet/minecraft/entity/player/InventoryPlayer;" };
			fields[mappingFields::lowerChestInventory] = { "lowerChestInventory", "Lnet/minecraft/inventory/IInventory;" };
			fields[mappingFields::upperChestInventory] = { "upperChestInventory", "Lnet/minecraft/inventory/IInventory;" };
			fields[mappingFields::itemDamage] = { "itemDamage", "I" };

			methods[mapping::getStackInSlot] = { "getStackInSlot", "(I)Lnet/minecraft/item/ItemStack;" };
			methods[mapping::getItem] = { "getItem", "()Lnet/minecraft/item/Item;" };
			methods[mapping::getInventory] = { "getInventory", "()[Lnet/minecraft/item/ItemStack;" };
			methods[mapping::getBlockPos] = { "getBlockPos", "()Lnet/minecraft/util/BlockPos;" };

			methods[mapping::getRotationYaw] = { "bridge$getRotationYaw", "()D" };
			methods[mapping::getRotationPitch] = { "bridge$getRotationPitch", "()D" };

			methods[mapping::getBBox] = { "bridge$getBoundingBox", "SIGNATURE NOT FOUND" };
			if (!getsig(mapping::getBBox, "bridge$getBoundingBox", entity_class, env))
				LOGERROR("can't find bbox");

			methods[mapping::getName] = { "getName", "()Ljava/lang/String;" };

			methods[mapping::getMotionX] = { "bridge$getMotionX", "()D" };
			methods[mapping::getMotionY] = { "bridge$getMotionY", "()D" };
			methods[mapping::getMotionZ] = { "bridge$getMotionZ", "()D" };

			methods[mapping::setRotationYaw] = { "bridge$setRotationPitch", "(D)V" };
			methods[mapping::setRotationPitch] = { "bridge$setRotationYaw", "(D)V" };
			methods[mapping::setRotation] = { "setRotation", "(FF)V" };
			
			methods[mapping::isInvisible] = { "isInvisible", "()Z" };

			// .EntityBaseLiving
			methods[mapping::getHeldItem] = { "getHeldItem", "()Lnet/minecraft/item/ItemStack;" };
			methods[mapping::getHurtTime] = { "bridge$getHurtTime", "()I" };
			methods[mapping::getHealth] = { "getHealth", "()F" };

			// .ActiveRenderInfo
			methods[mapping::getRenderPos] = { "getPosition", "()Lnet/minecraft/util/Vec3;" };

			fields[mappingFields::modelviewField] = { "MODELVIEW", "Ljava/nio/FloatBuffer;" };
			fields[mappingFields::viewportField] = { "VIEWPORT", "Ljava/nio/IntBuffer;" };
			fields[mappingFields::projectionField] = { "PROJECTION", "Ljava/nio/FloatBuffer;" };

			// ENTITY RENDERER
			methods[mapping::enableLightmap] = { "enableLightmap", "()V" };
			methods[mapping::disableLightmap] = { "disableLightmap", "()V" };

			// Vec3 class
			methods[mapping::Vec3Init] = { "<init>", "(DDD)V" };
			fields[mappingFields::Vec3X] = { "xCoord", "D" };
			fields[mappingFields::Vec3Y] = { "yCoord", "D" };
			fields[mappingFields::Vec3Z] = { "zCoord", "D" };

			// Vec3I class
			methods[mapping::Vec3IInit] = { "<init>", "(III)V" };
			methods[mapping::Vec3IX] = { "getX", "()I" };
			methods[mapping::Vec3IY] = { "getY", "()I" };
			methods[mapping::Vec3IZ] = { "getZ", "()I" };

			// bounding box
			methods[mapping::bboxMinX] = { "bridge$getMinX", "()D"};
			methods[mapping::bboxMinY] = { "bridge$getMinY", "()D"};
			methods[mapping::bboxMinZ] = { "bridge$getMinZ", "()D"};
			methods[mapping::bboxMaxX] = { "bridge$getMaxX", "()D"};
			methods[mapping::bboxMaxY] = { "bridge$getMaxY", "()D"};
			methods[mapping::bboxMaxZ] = { "bridge$getMaxZ", "()D"};

			// Timer
			fields[mappingFields::renderPartialTickField] = { "renderPartialTicks", "F" };
			fields[mappingFields::elapsedPartialTicks] = { "elapsedPartialTicks", "F" };
		}
		else if (client == toad::MC_CLIENT::Lunar_171)
		{
			fields[mappingFields::theMcField] = { "theMinecraft", "Lnet/minecraft/client/Minecraft;" };
			fields[mappingFields::timer] = { "timer", "Lnet/minecraft/util/Timer;" };

			fields[mappingFields::theWorldField ] = { "theWorld", "Lnet/minecraft/client/multiplayer/WorldClient;" };
			fields[mappingFields::objMouseOver] = { "objectMouseOver", "Lnet/minecraft/util/MovingObjectPosition;" };

			methods[mapping::getBlock] = { "getBlock", "()Lnet/minecraft/block/Block;" };

			//methods[mapping::getPlayer] = { "bridge$getPlayer", "SIGNATURE NOT FOUND" };
			//if (!getsig(mapping::getPlayer, "bridge$getPlayer", mcclass, env))
			//	LOGERROR("[mappings] Can't find player");// getPlayer, "()Lcom/moonsworth/lunar/IRRRCCICICRRRCRRRCOCOCIHI/HRRCROCRCIIHIOORRIIORRHCC/CCCHHICHCROHROCICOHCHHCOI/IRCOHCCIHIHRRRRRIIRHCRIHR;" });

			fields[mappingFields::thePlayerField] = { "thePlayer", "Lnet/minecraft/client/entity/EntityClientPlayerMP;" };
			fields[mappingFields::gameSettings] = { "gameSettings", "Lnet/minecraft/client/settings/GameSettings;" };

			fields[mappingFields::currentScreenField] = { "currentScreen", "Lnet/minecraft/client/gui/GuiScreen;" };
			fields[mappingFields::leftClickCounterField] = { "leftClickCounter", "I" };

			methods[mapping::getEntityHit] = { "bridge$getEntityHit", "" };
			// methodsig is in minecraft.cpp

			// General
			methods[mapping::toString] = { "toString", "()Ljava/lang/String;" };
			methods[mapping::getDisplayName] = { "getDisplayName", "()Ljava/lang/String;" };

			// WORLD
			methods[mapping::getPlayerEntities] = { "bridge$getPlayerEntities", "()Ljava/util/List;" };
			methods[mapping::isAirBlock] = { "isAirBlock", "(Lnet/minecraft/util/BlockPos;)Z" };

			methods[mapping::getRenderManager] = { "bridge$getRenderManager", "SIGNATURE NOT FOUND" };
			if (!getsig(mapping::getRenderManager, "bridge$getRenderManager", mcclass, env))
				LOGERROR("[mappings] Can't find render manager");

			fields[mappingFields::playerEntitiesField] = { "playerEntities", "Ljava/util/List;" };

			// .Block
			methods[mapping::getBlockFromBlockState] = { "getBlock", "()Lnet/minecraft/block/Block;" };
			methods[mapping::getIdFromBlockStatic] = { "getIdFromBlock", "(Lnet/minecraft/block/Block;)I" };

			// .GameSettings
			methods[mapping::setGamma] = { "bridge$setGamma", "(F)V" };

			fields[mappingFields::fovField] = { "fovSetting", "F" };

			// ENTITY
			fields[mappingFields::rotationYawField] = { "rotationYaw", "F" };
			fields[mappingFields::rotationPitchField] = { "rotationPitch", "F" };
			fields[mappingFields::prevRotationYawField] = { "prevRotationYaw", "F" };
			fields[mappingFields::prevRotationPitchField] = { "prevRotationPitch", "F" };

			fields[mappingFields::motionXField] = { "motionX", "D" };
			fields[mappingFields::motionYField] = { "motionY", "D" };
			fields[mappingFields::motionZField] = { "motionZ", "D" };

			fields[mappingFields::EntityPosX] = { "posX", "D" };
			fields[mappingFields::EntityPosY] = { "posY", "D" };
			fields[mappingFields::EntityPosZ] = { "posZ", "D" };

			fields[mappingFields::hurtTimeI] = { "hurtTime", "I" };

			fields[mappingFields::lastTickPosXField] = { "lastTickPosX", "D" };
			fields[mappingFields::lastTickPosYField] = { "lastTickPosY", "D" };
			fields[mappingFields::lastTickPosZField] = { "lastTickPosZ", "D" };

			fields[mappingFields::renderPartialTickField] = { "renderPartialTicks", "F" };

			methods[mapping::getPos] = { "getPosition", "(F)Lnet/minecraft/util/Vec3;" };
			methods[mapping::getBlockPosition] = { "getPosition", "()Lnet/minecraft/util/BlockPos;" };
			methods[mapping::isSneaking] = { "isSneaking", "()Z" };

			fields[mappingFields::inventoryField] = { "inventory", "Lnet/minecraft/entity/player/InventoryPlayer;" };
			fields[mappingFields::lowerChestInventory] = { "lowerChestInventory", "Lnet/minecraft/inventory/IInventory;" };
			fields[mappingFields::upperChestInventory] = { "upperChestInventory", "Lnet/minecraft/inventory/IInventory;" };
			fields[mappingFields::itemDamage] = { "itemDamage", "I" };

			methods[mapping::getInventory] = { "getInventory", "()[Lnet/minecraft/item/ItemStack;" };
			methods[mapping::getStackInSlot] = { "getStackInSlot", "(I)Lnet/minecraft/item/ItemStack;" };
			methods[mapping::getItem] = { "getItem", "()Lnet/minecraft/item/Item;" };

			methods[mapping::getBlockPos] = { "getBlockPos", "()Lnet/minecraft/util/BlockPos;" };

			methods[mapping::getRotationYaw] = { "bridge$getRotationYaw", "()D" };
			methods[mapping::getRotationPitch] = { "bridge$getRotationPitch", "()D" };

			methods[mapping::getBBox] = { "bridge$getBoundingBox", "SIGNATURE NOT FOUND" };
			if (!getsig(mapping::getBBox, "bridge$getBoundingBox", entity_class, env))
				LOGERROR("[mappings] can't find bbox");

			methods[mapping::getName] = { "bridge$getDisplayName", "()Ljava/lang/String;" };

			methods[mapping::getMotionX] = { "bridge$getMotionX", "()D" };
			methods[mapping::getMotionY] = { "bridge$getMotionY", "()D" };
			methods[mapping::getMotionZ] = { "bridge$getMotionZ", "()D" };

			methods[mapping::setRotationYaw] = { "bridge$setRotationPitch", "(D)V" };
			methods[mapping::setRotationPitch] = { "bridge$setRotationYaw", "(D)V" };
			methods[mapping::setRotation] = { "setRotation", "(FF)V" };

			methods[mapping::isInvisible] = { "isInvisible", "()Z" };

			// .EntityBaseLiving
			methods[mapping::getHeldItem] = { "getHeldItem", "()Lnet/minecraft/item/ItemStack;" };
			methods[mapping::getHurtTime] = { "bridge$getHurtTime", "()I" };
			methods[mapping::getHealth] = { "getHealth", "()F" };

			// .ActiveRenderInfo
			methods[mapping::getRenderPos] = { "getPosition", "()Lnet/minecraft/util/Vec3;" };

			fields[mappingFields::modelviewField] = { "modelview", "Ljava/nio/FloatBuffer;" };
			fields[mappingFields::viewportField] = { "viewport", "Ljava/nio/IntBuffer;" };
			fields[mappingFields::projectionField] = { "projection", "Ljava/nio/FloatBuffer;" };

			// ENTITY RENDERER
			methods[mapping::enableLightmap] = { "enableLightmap", "()V" };
			methods[mapping::disableLightmap] = { "disableLightmap", "()V" };

			// Vec3 class
			fields[mappingFields::Vec3X] = { "xCoord", "D" };
			fields[mappingFields::Vec3Y] = { "yCoord", "D" };
			fields[mappingFields::Vec3Z] = { "zCoord", "D" };

			// Vec3I class
			methods[mapping::Vec3IX] = { "getX", "()I" };
			methods[mapping::Vec3IY] = { "getY", "()I" };
			methods[mapping::Vec3IZ] = { "getZ", "()I" };

			// bounding box
			methods[mapping::bboxMinX] = { "bridge$getMinX", "()D" };
			methods[mapping::bboxMinY] = { "bridge$getMinY", "()D" };
			methods[mapping::bboxMinZ] = { "bridge$getMinZ", "()D" };
			methods[mapping::bboxMaxX] = { "bridge$getMaxX", "()D" };
			methods[mapping::bboxMaxY] = { "bridge$getMaxY", "()D" };
			methods[mapping::bboxMaxZ] = { "bridge$getMaxZ", "()D" };

			// Timer
			methods[mapping::partialTick] = { "bridge$partialTick", "()F" };
		}
		else
		{
			// try to get mappings using the mapping generator output data 
		}
	}

	bool getsig(mapping map, std::string_view name, const jclass klass, JNIEnv* env)
	{
#ifdef ENABLE_LOGGING
		std::vector<std::pair<std::string, std::string>> methods_data;
		static std::set<jclass> klasses;
#endif 

		for (int i = 0; i < jvmfunc::oJVM_GetClassMethodsCount(env, klass); i++)
		{
#ifdef ENABLE_LOGGING
			methods_data.emplace_back(jvmfunc::oJVM_GetMethodIxNameUTF(env, klass, i), jvmfunc::oJVM_GetMethodIxSignatureUTF(env, klass, i));
#endif 

			if (strcmp(jvmfunc::oJVM_GetMethodIxNameUTF(env, klass, i), name.data()) == 0)
			{
				methods[map].sig = jvmfunc::oJVM_GetMethodIxSignatureUTF(env, klass, i);
				//std::cout << name << " = " << std::string(jvmfunc::oJVM_GetMethodIxNameUTF(env, mcclass, i)) << " sig: " << jvmfunc::oJVM_GetMethodIxSignatureUTF(env, mcclass, i) << std::endl;
				//methodsigs.insert({ map, jvmfunc::oJVM_GetMethodIxSignatureUTF(env, klass, i) });
				return true;
			}
		}

#ifdef ENABLE_LOGGING
		if (klasses.contains(klass))
			return false;

		LOGDEBUG("[getsig] methods data, {}", name);

		for (const auto& [name, sig] : methods_data)
		{
			LOGDEBUG("[getsig] {} {} ", name, sig);
		}
		klasses.emplace(klass);
#endif 
		return false;
	}
}
