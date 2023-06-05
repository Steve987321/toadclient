#pragma once

namespace toadll::modules
{
	inline std::vector<std::thread> threads;

	// Initialized the module threads
	void initialize();

	// @TODO: remove
	void updateTick();

	// @TODO: make a module ? 
	void auto_bridge(const std::shared_ptr<c_Entity>& lPlayer);

}