#pragma once

// all cheat functions 
namespace toadll::modules
{
	//inline std::vector<vec3> entitiepositions;
	//inline double renderposX, renderposY, renderposZ;
	//inline double yaw, pitch;

	inline std::shared_ptr<c_Entity> lPlayer;

	void update();

	void aa(const std::shared_ptr<c_Entity>& lPlayer);
	void esp(const vec3& ePos);
	void auto_bridge();

}