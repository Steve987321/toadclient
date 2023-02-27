#pragma once

// all cheat functions 
namespace toadll::modules
{
	//inline std::vector<vec3> entitiepositions;
	//inline double renderposX, renderposY, renderposZ;
	//inline double yaw, pitch;

	void update();

	void velocity(const std::shared_ptr<c_Entity>& lPlayer);
	void aa(const std::shared_ptr<c_Entity>& lPlayer);
	void esp(const vec3& ePos);
	void auto_bridge(const std::shared_ptr<c_Entity>& lPlayer);

}