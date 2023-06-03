#pragma once

namespace toadll
{

	class CVelocity SET_MODULE_CLASS(CVelocity)
	{
	public:
		void OnTick(const std::shared_ptr<c_Entity>& lPlayer) override;
	};

}

