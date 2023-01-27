#include "pch.h"
#include "Toad/Toad.h"
#include "modules.h"

void toadll::modules::aa()
{

}

void toadll::modules::esp()
{
}

void toadll::modules::edge_sneak()
{
	auto world = p_Minecraft->get_world();
	if (!world) return;

	

	if (env->CallBooleanMethod(world, get_mid(world, mapping::isAirBlock), ))
	{
		
	}
}
