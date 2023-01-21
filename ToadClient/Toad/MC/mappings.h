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

	inline void init_map(const minecraft_client& client)
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
			methodsigs.insert({ mapping::getWorld, "()Lcom/moonsworth/lunar/OCIROORRIRIOHIIIRRIORCIHI/RCHHIHHRCCIHIRCCCRHIOCCHR/RCHHIHHRCCIHIRCCCRHIOCCHR/IIROHIIHCHOORORIIHORHRORO;" });

			methodnames.insert({ mapping::getPlayer, "bridge$getPlayer" });
			methodsigs.insert({ mapping::getPlayer, "()Lcom/moonsworth/lunar/OCIROORRIRIOHIIIRRIORCIHI/RCHHIHHRCCIHIRCCCRHIOCCHR/OCRHRIOCIICCRHCHHHOHHCROC/OIOORRORCRHOOCICORRCHOHRC;" });

			methodnames.insert({ mapping::getPos, "getPositionVector" });
			methodsigs.insert({ mapping::getPos, "()Lnet/minecraft/util/Vec3;" });

			// vec3
			methodnames.insert({ mapping::Vec3X, "bridge$xCoord" });
			methodsigs.insert({ mapping::Vec3X, "()D" });
			methodnames.insert({ mapping::Vec3Y, "bridge$yCoord" });
			methodsigs.insert({ mapping::Vec3Y, "()D" });
			methodnames.insert({ mapping::Vec3Z, "bridge$zCoord" });
			methodsigs.insert({ mapping::Vec3Z, "()D" });
		}
	}
}
