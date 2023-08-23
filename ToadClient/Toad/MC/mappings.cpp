#include "pch.h"
#include "Toad/Toad.h"
#include "mappings.h"

namespace toadll::mappings
{
	const char* findName(mapping name)
	{
		return methodnames.find(name)->second;
	}

	const char* findSig(mapping name)
	{
		return methodsigs.find(name)->second;
	}

	const char* findNameField( mappingFields name)
	{
		return fieldnames.find(name)->second;
	}

	const char* findSigField(mappingFields name)
	{
		return fieldsigs.find(name)->second;
	}

	void init_map(JNIEnv* env, jclass mcclass, jclass entity_class, toad::MC_CLIENT client)
	{
		if (client == toad::MC_CLIENT::Vanilla)
		{
			
		}
		else if (client == toad::MC_CLIENT::Forge)
		{

		}
		else if (client == toad::MC_CLIENT::Lunar_189)
		{
			methodnames.insert({ mapping::getMinecraft, "getMinecraft" });
			methodsigs.insert({ mapping::getMinecraft, "()Lnet/minecraft/client/Minecraft;" });

			methodnames.insert({ mapping::getWorld, "bridge$getWorld" });
			if (!getsig(mapping::getWorld, "bridge$getWorld", mcclass, env))
				LOGERROR("can't find world"); //methodsigs.insert({ mapping::getWorld, "()Lcom/moonsworth/lunar/IRRRCCICICRRRCRRRCOCOCIHI/HRRCROCRCIIHIOORRIIORRHCC/HRRCROCRCIIHIOORRIIORRHCC/HORIRCRCHHRHIORIHRRRIHIIH;" });

			fieldnames.insert({ mappingFields::theWorldField , "theWorld" });
			fieldsigs.insert({ mappingFields::theWorldField , "Lnet/minecraft/client/multiplayer/WorldClient;" });

			fieldnames.insert({ mappingFields::objMouseOver, "objectMouseOver" });
			fieldsigs.insert({ mappingFields::objMouseOver, "Lnet/minecraft/util/MovingObjectPosition;" });
			// get world class (temp)
			auto worldclass = findclass("net.minecraft.world.World", env);
			
			methodnames.insert({ mapping::getBlockAt, "bridge$getBlockAt" });
			if (!getsig(mapping::getBlockAt, "bridge$getBlockAt", worldclass, env))
				LOGERROR("can't find getBlockAt");

			env->DeleteLocalRef(worldclass);

			methodnames.insert({ mapping::getPlayer, "bridge$getPlayer" });
			if (!getsig(mapping::getPlayer, "bridge$getPlayer", mcclass, env))
				LOGERROR("can't find player");// methodsigs.insert({ mapping::getPlayer, "()Lcom/moonsworth/lunar/IRRRCCICICRRRCRRRCOCOCIHI/HRRCROCRCIIHIOORRIIORRHCC/CCCHHICHCROHROCICOHCHHCOI/IRCOHCCIHIHRRRRRIIRHCRIHR;" });

			fieldnames.insert({mappingFields::thePlayerField, "thePlayer"});
			fieldsigs.insert({mappingFields::thePlayerField, "Lnet/minecraft/client/entity/EntityPlayerSP;"});

			methodnames.insert({ mapping::getGameSettings, "bridge$getGameSettings" });
			if (!getsig(mapping::getGameSettings, "bridge$getGameSettings", mcclass, env))
				LOGERROR("can't find gamesettings");

			methodnames.insert({ mapping::getObjectMouseOver, "bridge$getObjectMouseOver" });
			if (!getsig(mapping::getObjectMouseOver, "bridge$getObjectMouseOver", mcclass, env))
				LOGERROR("can't find getobjectmouseover");// methodsigs.insert({ mapping::getObjectMouseOver, "()Lcom/moonsworth/lunar/IRRRCCICICRRRCRRRCOCOCIHI/CHOOIIHOCOHCHIIRIOHCIOCOH/IHRRCCOCORIIROHOCCCOCHCOI;" });

			methodnames.insert({ mapping::getEntityRenderer, "bridge$getEntityRenderer" });
			if (!getsig(mapping::getEntityRenderer, "bridge$getEntityRenderer", mcclass, env))
				LOGERROR("can't find entityRenderer");// methodsigs.insert({ mapping::getEntityRenderer, "()Lcom/moonsworth/lunar/IHORCOOHCIIHOHOOIHHRRHOCH/ORCIIICOHRRHCRCRRIRCCRIRR/IOHIIHOIORCROROCCHIHRCCHI/RHCOOOOHOIOCIHROHHCROHIOC/OOOCHCRHOCOCROIOOCHIRIOOR;" });

			methodnames.insert({ mapping::getTimer, "bridge$getTimer" });
			if (!getsig(mapping::getTimer, "bridge$getTimer", mcclass, env))
				LOGERROR("can't find timer");

			fieldnames.insert({mappingFields::currentScreenField, "currentScreen"});
			fieldsigs.insert({mappingFields::currentScreenField, "Lnet/minecraft/client/gui/GuiScreen;"});

			fieldnames.insert({ mappingFields::leftClickCounterField, "leftClickCounter" });
			fieldsigs.insert({ mappingFields::leftClickCounterField, "I" });

			methodnames.insert({ mapping::getEntityHit, "bridge$getEntityHit" });
			// methodsig is in minecraft.cpp

			// General
			methodnames.insert({ mapping::toString, "toString" });
			methodsigs.insert({ mapping::toString, "()Ljava/lang/String;" });

			// WORLD
			methodnames.insert({ mapping::getPlayerEntities, "bridge$getPlayerEntities" });
			methodsigs.insert({ mapping::getPlayerEntities, "()Ljava/util/List;" });

			fieldnames.insert({ mappingFields::playerEntitiesField, "playerEntities" });
			fieldsigs.insert({ mappingFields::playerEntitiesField, "Ljava/util/List;" });

			methodnames.insert({ mapping::getRenderManager, "getRenderManager" });
			methodsigs.insert({ mapping::getRenderManager, "()Lnet/minecraft/client/renderer/entity/RenderManager;" });

			methodnames.insert({ mapping::isAirBlock, "isAirBlock" });
			methodsigs.insert({ mapping::isAirBlock, "(Lnet/minecraft/util/BlockPos;)Z" });

			methodnames.insert({ mapping::rayTraceBlocks, "rayTraceBlocks" });
			methodsigs.insert({ mapping::rayTraceBlocks, "(Lnet/minecraft/util/Vec3;Lnet/minecraft/util/Vec3;Z)Lnet/minecraft/util/MovingObjectPosition;" });

			// .Block
			methodnames.insert({ mapping::getBlockFromBlockState, "getBlock" });
			methodsigs.insert({ mapping::getBlockFromBlockState, "()Lnet/minecraft/block/Block;" });

			methodnames.insert({ mapping::getIdFromBlockStatic, "getIdFromBlock" });
			methodsigs.insert({ mapping::getIdFromBlockStatic, "(Lnet/minecraft/block/Block;)I" });

			


			// Player
			//auto mcobj = env->CallStaticObjectMethod(mcclass, get_static_mid(mcclass, mapping::getMinecraft));
			//auto player = env->CallObjectMethod(mcobj, get_mid(mcobj, mapping::getPlayer));
			//auto playerclass = env->GetObjectClass(player);

			//methodnames.insert({ mapping::getOpenContainer, "bridge$getOpenContainer" });
			//if (!getsig(mapping::getOpenContainer, "bridge$getOpenContainer", playerclass))
			//	log_Error("can't find getinventory"); // methodsigs.insert(mapping::getOpenContainer, "bridge$getOpenContainer");

			//env->DeleteLocalRef(mcobj);
			//env->DeleteLocalRef(player);
			//env->DeleteLocalRef(playerclass);

			// .GameSettings
			methodnames.insert({ mapping::setGamma, "bridge$setGamma" });
			methodsigs.insert({ mapping::setGamma, "(F)V" });

			fieldnames.insert({ mappingFields::fovField, "fovSetting" });
			fieldsigs.insert({ mappingFields::fovField, "F" });

			// ENTITY
			fieldnames.insert({ mappingFields::rotationYawField, "rotationYaw" });
			fieldsigs.insert({ mappingFields::rotationYawField, "F" });
			
			fieldnames.insert({ mappingFields::rotationPitchField, "rotationPitch" });
			fieldsigs.insert({ mappingFields::rotationPitchField, "F" });

			fieldnames.insert({ mappingFields::prevRotationYawField, "prevRotationYaw" });
			fieldsigs.insert({ mappingFields::prevRotationYawField, "F" });

			fieldnames.insert({ mappingFields::prevRotationPitchField, "prevRotationPitch" });
			fieldsigs.insert({ mappingFields::prevRotationPitchField, "F" });

			fieldnames.insert({ mappingFields::motionXField, "motionX" });
			fieldnames.insert({ mappingFields::motionYField, "motionY" });
			fieldnames.insert({ mappingFields::motionZField, "motionZ" });
			fieldsigs.insert({ mappingFields::motionXField, "D" });
			fieldsigs.insert({ mappingFields::motionYField, "D" });
			fieldsigs.insert({ mappingFields::motionZField, "D" });

			fieldnames.insert({ mappingFields::EntityPosX, "posX" });
			fieldnames.insert({ mappingFields::EntityPosY, "posY" });
			fieldnames.insert({ mappingFields::EntityPosZ, "posZ" });
			fieldsigs.insert({ mappingFields::EntityPosX, "D" });
			fieldsigs.insert({ mappingFields::EntityPosY, "D" });
			fieldsigs.insert({ mappingFields::EntityPosZ, "D" });

			fieldnames.insert({ mappingFields::hurtTimeI, "hurtTime" });
			fieldsigs.insert({ mappingFields::hurtTimeI, "I" });

			fieldnames.insert({ mappingFields::lastTickPosXField, "lastTickPosX" });
			fieldsigs.insert({ mappingFields::lastTickPosXField, "D" });
			fieldnames.insert({ mappingFields::lastTickPosYField, "lastTickPosY" });
			fieldsigs.insert({ mappingFields::lastTickPosYField, "D" });
			fieldnames.insert({ mappingFields::lastTickPosZField, "lastTickPosZ" });
			fieldsigs.insert({ mappingFields::lastTickPosZField, "D" });

			fieldnames.insert({ mappingFields::renderPartialTickField, "renderPartialTicks" });
			fieldsigs.insert({ mappingFields::renderPartialTickField, "F" });

			methodnames.insert({ mapping::getPos, "getPositionVector" });
			methodsigs.insert({ mapping::getPos, "()Lnet/minecraft/util/Vec3;" });

			methodnames.insert({ mapping::getBlockPosition, "getPosition" });
			methodsigs.insert({ mapping::getBlockPosition, "()Lnet/minecraft/util/BlockPos;" });

			auto movingblockklass = findclass("net.minecraft.util.MovingObjectPosition", env);
			methodnames.insert({ mapping::getBlockPositionFromMovingBlock, "bridge$getBlockPosition" });
			if (!getsig(mapping::getBlockPositionFromMovingBlock, "bridge$getBlockPosition", movingblockklass, env))
				LOGERROR("can't find bridge$getBlockPosition from moving block class");

			env->DeleteLocalRef(movingblockklass);

			std::cout << methodsigs[mapping::getBlockPositionFromMovingBlock] << std::endl;

			fieldnames.insert({ mappingFields::inventoryField, "inventory" });
			fieldsigs.insert({ mappingFields::inventoryField, "Lnet/minecraft/entity/player/InventoryPlayer;" });

			/*fieldnames.insert({ mappingFields::mainInventoryField, "mainInventory" });
			fieldsigs.insert({ mappingFields::mainInventoryField, "Ljava/util/List;" });*/
			methodnames.insert({ mapping::getStackInSlot, "getStackInSlot" });
			methodsigs.insert({ mapping::getStackInSlot, "(I)Lnet/minecraft/item/ItemStack;" });

			methodnames.insert({ mapping::getInventory, "getInventory" });
			methodsigs.insert({ mapping::getInventory, "()[Lnet/minecraft/item/ItemStack;" });

			methodnames.insert({ mapping::getBlockPos, "getBlockPos" });
			methodsigs.insert({ mapping::getBlockPos, "()Lnet/minecraft/util/BlockPos;" });

			fieldnames.insert({ mappingFields::blockPos, "blockPos" });
			fieldnames.insert({ mappingFields::blockPos, "Lnet/minecraft/util/BlockPos;" });

			methodnames.insert({ mapping::getRotationYaw, "bridge$getRotationYaw" });
			methodnames.insert({ mapping::getRotationPitch, "bridge$getRotationPitch" });
			methodsigs.insert({ mapping::getRotationYaw, "()D" });
			methodsigs.insert({ mapping::getRotationPitch, "()D" });

			methodnames.insert({ mapping::getBBox, "bridge$getBoundingBox" });
			if (!getsig(mapping::getBBox, "bridge$getBoundingBox", entity_class, env))
				LOGERROR("can't find bbox");

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
			methodnames.insert({ mapping::setRotation, "setRotation" });
			methodsigs.insert({ mapping::setRotation, "(FF)V" });
			
			methodnames.insert({ mapping::isInvisible, "isInvisible" });
			methodsigs.insert({ mapping::isInvisible, "()Z" });

			// .EntityBaseLiving
			methodnames.insert({ mapping::getHeldItem, "getHeldItem" });
			methodsigs.insert({ mapping::getHeldItem, "()Lnet/minecraft/item/ItemStack;" });

			methodnames.insert({ mapping::getHurtTime, "bridge$getHurtTime" });
			methodsigs.insert({ mapping::getHurtTime, "()I" });

			methodnames.insert({ mapping::getHealth, "getHealth" });
			methodsigs.insert({ mapping::getHealth, "()F" });

			// .ActiveRenderInfo
			methodnames.insert({ mapping::getRenderPos, "getPosition" });
			methodsigs.insert({ mapping::getRenderPos, "()Lnet/minecraft/util/Vec3;" });

			fieldnames.insert({ mappingFields::modelviewField, "MODELVIEW" });
			fieldsigs.insert({ mappingFields::modelviewField, "Ljava/nio/FloatBuffer;" });
			fieldnames.insert({ mappingFields::viewportField, "VIEWPORT" });
			fieldsigs.insert({ mappingFields::viewportField, "Ljava/nio/IntBuffer;" });
			fieldnames.insert({ mappingFields::projectionField, "PROJECTION" });
			fieldsigs.insert({ mappingFields::projectionField, "Ljava/nio/FloatBuffer;" });

			//// .RenderManager
			//methodnames.insert({ mapping::renderManGetRenderPosX, "bridge$viewerPosX" });
			//methodsigs.insert({ mapping::renderManGetRenderPosY, "()D" });
			//methodnames.insert({ mapping::renderManGetRenderPosZ, "bridge$viewerPosY" });
			//methodsigs.insert({ mapping::renderManGetRenderPosX, "()D" });
			//methodnames.insert({ mapping::renderManGetRenderPosY, "bridge$viewerPosZ" });
			//methodsigs.insert({ mapping::renderManGetRenderPosZ, "()D" });

			// ENTITY RENDERER
			methodnames.insert({ mapping::enableLightmap, "enableLightmap" });
			methodsigs.insert({ mapping::enableLightmap, "()V" });
			methodnames.insert({ mapping::disableLightmap, "disableLightmap" });
			methodsigs.insert({ mapping::disableLightmap, "()V" });

			// Vec3 class
			methodnames.insert({ mapping::Vec3Init, "<init>" });
			methodsigs.insert({ mapping::Vec3Init, "(DDD)V" });
			methodnames.insert({ mapping::Vec3X, "bridge$xCoord" });
			methodsigs.insert({ mapping::Vec3X, "()D" });
			methodnames.insert({ mapping::Vec3Y, "bridge$yCoord" });
			methodsigs.insert({ mapping::Vec3Y, "()D" });
			methodnames.insert({ mapping::Vec3Z, "bridge$zCoord" });
			methodsigs.insert({ mapping::Vec3Z, "()D" });

			// Vec3I class
			methodnames.insert({ mapping::Vec3IInit, "<init>" });
			methodsigs.insert({ mapping::Vec3IInit, "(III)V" });
			methodnames.insert({ mapping::Vec3IX, "getX" });
			methodsigs.insert({ mapping::Vec3IX, "()I" });
			methodnames.insert({ mapping::Vec3IY, "getY" });
			methodsigs.insert({ mapping::Vec3IY, "()I" });
			methodnames.insert({ mapping::Vec3IZ, "getZ" });
			methodsigs.insert({ mapping::Vec3IZ, "()I" });

			// bounding box
			methodnames.insert({mapping::bboxMinX, "bridge$getMinX"});
			methodnames.insert({mapping::bboxMinY, "bridge$getMinY"});
			methodnames.insert({mapping::bboxMinZ, "bridge$getMinZ"});
			methodnames.insert({mapping::bboxMaxX, "bridge$getMaxX"});
			methodnames.insert({mapping::bboxMaxY, "bridge$getMaxY"});
			methodnames.insert({mapping::bboxMaxZ, "bridge$getMaxZ"});

			methodsigs.insert({ mapping::bboxMinX, "()D" });
			methodsigs.insert({ mapping::bboxMinY, "()D" });
			methodsigs.insert({ mapping::bboxMinZ, "()D" });
			methodsigs.insert({ mapping::bboxMaxX, "()D" });
			methodsigs.insert({ mapping::bboxMaxY, "()D" });
			methodsigs.insert({ mapping::bboxMaxZ, "()D" });

			// Timer
			methodnames.insert({ mapping::partialTick, "bridge$partialTick" });
			methodsigs.insert({ mapping::partialTick, "()F" });
		}
		else if (client == toad::MC_CLIENT::Lunar_171)
		{
			methodnames.insert({ mapping::getMinecraft, "getMinecraft" });
			methodsigs.insert({ mapping::getMinecraft, "()Lnet/minecraft/client/Minecraft;" });

			methodnames.insert({ mapping::getWorld, "bridge$getWorld" });
			if (!getsig(mapping::getWorld, "bridge$getWorld", mcclass, env))
				LOGERROR("can't find world"); //methodsigs.insert({ mapping::getWorld, "()Lcom/moonsworth/lunar/IRRRCCICICRRRCRRRCOCOCIHI/HRRCROCRCIIHIOORRIIORRHCC/HRRCROCRCIIHIOORRIIORRHCC/HORIRCRCHHRHIORIHRRRIHIIH;" });

			fieldnames.insert({ mappingFields::theWorldField , "theWorld" });
			fieldsigs.insert({ mappingFields::theWorldField , "Lnet/minecraft/client/multiplayer/WorldClient;" });

			fieldnames.insert({ mappingFields::objMouseOver, "objectMouseOver" });
			fieldsigs.insert({ mappingFields::objMouseOver, "Lnet/minecraft/util/MovingObjectPosition;" });

			// get world class (temp)
			auto worldclass = findclass("net.minecraft.world.World", env);

			methodnames.insert({ mapping::getBlockAt, "bridge$getBlockAt" });
			if (!getsig(mapping::getBlockAt, "bridge$getBlockAt", worldclass, env))
				LOGERROR("can't find getBlockAt");

			env->DeleteLocalRef(worldclass);

			methodnames.insert({ mapping::getPlayer, "bridge$getPlayer" });
			if (!getsig(mapping::getPlayer, "bridge$getPlayer", mcclass, env))
				LOGERROR("can't find player");// methodsigs.insert({ mapping::getPlayer, "()Lcom/moonsworth/lunar/IRRRCCICICRRRCRRRCOCOCIHI/HRRCROCRCIIHIOORRIIORRHCC/CCCHHICHCROHROCICOHCHHCOI/IRCOHCCIHIHRRRRRIIRHCRIHR;" });

			fieldnames.insert({ mappingFields::thePlayerField, "thePlayer" });
			fieldsigs.insert({ mappingFields::thePlayerField, "Lnet/minecraft/client/entity/EntityClientPlayerMP;" });

			methodnames.insert({ mapping::getGameSettings, "bridge$getGameSettings" });
			if (!getsig(mapping::getGameSettings, "bridge$getGameSettings", mcclass, env))
				LOGERROR("can't find gamesettings");

			methodnames.insert({ mapping::getObjectMouseOver, "bridge$getObjectMouseOver" });
			if (!getsig(mapping::getObjectMouseOver, "bridge$getObjectMouseOver", mcclass, env))
				LOGERROR("can't find getobjectmouseover");// methodsigs.insert({ mapping::getObjectMouseOver, "()Lcom/moonsworth/lunar/IRRRCCICICRRRCRRRCOCOCIHI/CHOOIIHOCOHCHIIRIOHCIOCOH/IHRRCCOCORIIROHOCCCOCHCOI;" });

			methodnames.insert({ mapping::getEntityRenderer, "bridge$getEntityRenderer" });
			if (!getsig(mapping::getEntityRenderer, "bridge$getEntityRenderer", mcclass, env))
				LOGERROR("can't find entityRenderer");// methodsigs.insert({ mapping::getEntityRenderer, "()Lcom/moonsworth/lunar/IHORCOOHCIIHOHOOIHHRRHOCH/ORCIIICOHRRHCRCRRIRCCRIRR/IOHIIHOIORCROROCCHIHRCCHI/RHCOOOOHOIOCIHROHHCROHIOC/OOOCHCRHOCOCROIOOCHIRIOOR;" });

			methodnames.insert({ mapping::getTimer, "bridge$getTimer" });
			if (!getsig(mapping::getTimer, "bridge$getTimer", mcclass, env))
				LOGERROR("can't find timer");

			fieldnames.insert({ mappingFields::currentScreenField, "currentScreen" });
			fieldsigs.insert({ mappingFields::currentScreenField, "Lnet/minecraft/client/gui/GuiScreen;" });

			fieldnames.insert({ mappingFields::leftClickCounterField, "leftClickCounter" });
			fieldsigs.insert({ mappingFields::leftClickCounterField, "I" });

			methodnames.insert({ mapping::getEntityHit, "bridge$getEntityHit" });
			// methodsig is in minecraft.cpp

			// General
			methodnames.insert({ mapping::toString, "toString" });
			methodsigs.insert({ mapping::toString, "()Ljava/lang/String;" });

			// WORLD
			methodnames.insert({ mapping::getPlayerEntities, "bridge$getPlayerEntities" });
			methodsigs.insert({ mapping::getPlayerEntities, "()Ljava/util/List;" });

			fieldnames.insert({ mappingFields::playerEntitiesField, "playerEntities" });
			fieldsigs.insert({ mappingFields::playerEntitiesField, "Ljava/util/List;" });

			methodnames.insert({ mapping::getRenderManager, "bridge$getRenderManager" });
			if (!getsig(mapping::getRenderManager, "bridge$getRenderManager", mcclass, env))
				LOGERROR("can't find render manager");

			methodnames.insert({ mapping::isAirBlock, "isAirBlock" });
			methodsigs.insert({ mapping::isAirBlock, "(Lnet/minecraft/util/BlockPos;)Z" });

			// .Block
			methodnames.insert({ mapping::getBlockFromBlockState, "getBlock" });
			methodsigs.insert({ mapping::getBlockFromBlockState, "()Lnet/minecraft/block/Block;" });

			methodnames.insert({ mapping::getIdFromBlockStatic, "getIdFromBlock" });
			methodsigs.insert({ mapping::getIdFromBlockStatic, "(Lnet/minecraft/block/Block;)I" });

			// Player
			//auto mcobj = env->CallStaticObjectMethod(mcclass, get_static_mid(mcclass, mapping::getMinecraft));
			//auto player = env->CallObjectMethod(mcobj, get_mid(mcobj, mapping::getPlayer));
			//auto playerclass = env->GetObjectClass(player);

			//methodnames.insert({ mapping::getOpenContainer, "bridge$getOpenContainer" });
			//if (!getsig(mapping::getOpenContainer, "bridge$getOpenContainer", playerclass))
			//	log_Error("can't find getinventory"); // methodsigs.insert(mapping::getOpenContainer, "bridge$getOpenContainer");

			//env->DeleteLocalRef(mcobj);
			//env->DeleteLocalRef(player);
			//env->DeleteLocalRef(playerclass);

			// .GameSettings
			methodnames.insert({ mapping::setGamma, "bridge$setGamma" });
			methodsigs.insert({ mapping::setGamma, "(F)V" });

			fieldnames.insert({ mappingFields::fovField, "fovSetting" });
			fieldsigs.insert({ mappingFields::fovField, "F" });

			// ENTITY
			fieldnames.insert({ mappingFields::rotationYawField, "rotationYaw" });
			fieldsigs.insert({ mappingFields::rotationYawField, "F" });

			fieldnames.insert({ mappingFields::rotationPitchField, "rotationPitch" });
			fieldsigs.insert({ mappingFields::rotationPitchField, "F" });

			fieldnames.insert({ mappingFields::prevRotationYawField, "prevRotationYaw" });
			fieldsigs.insert({ mappingFields::prevRotationYawField, "F" });

			fieldnames.insert({ mappingFields::prevRotationPitchField, "prevRotationPitch" });
			fieldsigs.insert({ mappingFields::prevRotationPitchField, "F" });

			fieldnames.insert({ mappingFields::motionXField, "motionX" });
			fieldnames.insert({ mappingFields::motionYField, "motionY" });
			fieldnames.insert({ mappingFields::motionZField, "motionZ" });
			fieldsigs.insert({ mappingFields::motionXField, "D" });
			fieldsigs.insert({ mappingFields::motionYField, "D" });
			fieldsigs.insert({ mappingFields::motionZField, "D" });

			fieldnames.insert({ mappingFields::EntityPosX, "posX" });
			fieldnames.insert({ mappingFields::EntityPosY, "posY" });
			fieldnames.insert({ mappingFields::EntityPosZ, "posZ" });
			fieldsigs.insert({ mappingFields::EntityPosX, "D" });
			fieldsigs.insert({ mappingFields::EntityPosY, "D" });
			fieldsigs.insert({ mappingFields::EntityPosZ, "D" });

			fieldnames.insert({ mappingFields::hurtTimeI, "hurtTime" });
			fieldsigs.insert({ mappingFields::hurtTimeI, "I" });

			fieldnames.insert({ mappingFields::lastTickPosXField, "lastTickPosX" });
			fieldsigs.insert({ mappingFields::lastTickPosXField, "D" });
			fieldnames.insert({ mappingFields::lastTickPosYField, "lastTickPosY" });
			fieldsigs.insert({ mappingFields::lastTickPosYField, "D" });
			fieldnames.insert({ mappingFields::lastTickPosZField, "lastTickPosZ" });
			fieldsigs.insert({ mappingFields::lastTickPosZField, "D" });

			fieldnames.insert({ mappingFields::renderPartialTickField, "renderPartialTicks" });
			fieldsigs.insert({ mappingFields::renderPartialTickField, "F" });

			methodnames.insert({ mapping::getPos, "getPosition" });
			methodsigs.insert({ mapping::getPos, "(F)Lnet/minecraft/util/Vec3;" });

			methodnames.insert({ mapping::getBlockPosition, "getPosition" });
			methodsigs.insert({ mapping::getBlockPosition, "()Lnet/minecraft/util/BlockPos;" });

			fieldnames.insert({ mappingFields::inventoryField, "inventory" });
			fieldsigs.insert({ mappingFields::inventoryField, "Lnet/minecraft/entity/player/InventoryPlayer;" });

			methodnames.insert({ mapping::getInventory, "getInventory" });
			methodsigs.insert({ mapping::getInventory, "()[Lnet/minecraft/item/ItemStack;" });
			/*fieldnames.insert({ mappingFields::mainInventoryField, "mainInventory" });
			fieldsigs.insert({ mappingFields::mainInventoryField, "Ljava/util/List;" });*/
			methodnames.insert({ mapping::getStackInSlot, "getStackInSlot" });
			methodsigs.insert({ mapping::getStackInSlot, "(I)Lnet/minecraft/item/ItemStack;" });


			methodnames.insert({ mapping::getBlockPos, "getBlockPos" });
			methodsigs.insert({ mapping::getBlockPos, "()Lnet/minecraft/util/BlockPos;" });

			methodnames.insert({ mapping::getRotationYaw, "bridge$getRotationYaw" });
			methodnames.insert({ mapping::getRotationPitch, "bridge$getRotationPitch" });
			methodsigs.insert({ mapping::getRotationYaw, "()D" });
			methodsigs.insert({ mapping::getRotationPitch, "()D" });

			methodnames.insert({ mapping::getBBox, "bridge$getBoundingBox" });
			if (!getsig(mapping::getBBox, "bridge$getBoundingBox", entity_class, env))
				LOGERROR("can't find bbox");

			methodnames.insert({ mapping::getName, "bridge$getDisplayName" });
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
			methodnames.insert({ mapping::setRotation, "setRotation" });
			methodsigs.insert({ mapping::setRotation, "(FF)V" });

			methodnames.insert({ mapping::isInvisible, "isInvisible" });
			methodsigs.insert({ mapping::isInvisible, "()Z" });

			// .EntityBaseLiving
			methodnames.insert({ mapping::getHeldItem, "getHeldItem" });
			methodsigs.insert({ mapping::getHeldItem, "()Lnet/minecraft/item/ItemStack;" });

			methodnames.insert({ mapping::getHurtTime, "bridge$getHurtTime" });
			methodsigs.insert({ mapping::getHurtTime, "()I" });

			methodnames.insert({ mapping::getHealth, "getHealth" });
			methodsigs.insert({ mapping::getHealth, "()F" });

			// .ActiveRenderInfo
			methodnames.insert({ mapping::getRenderPos, "getPosition" });
			methodsigs.insert({ mapping::getRenderPos, "()Lnet/minecraft/util/Vec3;" });

			fieldnames.insert({ mappingFields::modelviewField, "modelview" });
			fieldsigs.insert({ mappingFields::modelviewField, "Ljava/nio/FloatBuffer;" });
			fieldnames.insert({ mappingFields::viewportField, "viewport" });
			fieldsigs.insert({ mappingFields::viewportField, "Ljava/nio/IntBuffer;" });
			fieldnames.insert({ mappingFields::projectionField, "projection" });
			fieldsigs.insert({ mappingFields::projectionField, "Ljava/nio/FloatBuffer;" });

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

			// bounding box
			methodnames.insert({ mapping::bboxMinX, "bridge$getMinX" });
			methodnames.insert({ mapping::bboxMinY, "bridge$getMinY" });
			methodnames.insert({ mapping::bboxMinZ, "bridge$getMinZ" });
			methodnames.insert({ mapping::bboxMaxX, "bridge$getMaxX" });
			methodnames.insert({ mapping::bboxMaxY, "bridge$getMaxY" });
			methodnames.insert({ mapping::bboxMaxZ, "bridge$getMaxZ" });

			methodsigs.insert({ mapping::bboxMinX, "()D" });
			methodsigs.insert({ mapping::bboxMinY, "()D" });
			methodsigs.insert({ mapping::bboxMinZ, "()D" });
			methodsigs.insert({ mapping::bboxMaxX, "()D" });
			methodsigs.insert({ mapping::bboxMaxY, "()D" });
			methodsigs.insert({ mapping::bboxMaxZ, "()D" });

			// Timer
			methodnames.insert({ mapping::partialTick, "bridge$partialTick" });
			methodsigs.insert({ mapping::partialTick, "()F" });
		}
	}
}
