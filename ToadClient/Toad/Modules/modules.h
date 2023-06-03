#pragma once

namespace toadll::modules
{
	struct ModVars
	{
		const std::shared_ptr<c_Entity>& lPlayer;
		const std::shared_ptr<c_Minecraft>& MC;
	};

	void initialize();
	void update();
	void auto_bridge(const std::shared_ptr<c_Entity>& lPlayer);

}