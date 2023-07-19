#pragma once

namespace toadll
{

	class CVelocity SET_MODULE_CLASS(CVelocity)
	{
	public:
		void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;
	};

}

