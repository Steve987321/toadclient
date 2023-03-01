#pragma once

// all cheat functions 
namespace toadll::modules
{
	void update();

	void update_esp_vars(const std::shared_ptr<c_Entity>& lPlayer);

	void velocity(const std::shared_ptr<c_Entity>& lPlayer);
	void aa(const std::shared_ptr<c_Entity>& lPlayer);
	void auto_bridge(const std::shared_ptr<c_Entity>& lPlayer);

}