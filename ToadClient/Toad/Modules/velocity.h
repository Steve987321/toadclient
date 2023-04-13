#pragma once

namespace toadll
{

	class CVelocity SET_MODULE_CLASS(CVelocity)
	{
	private:
		float timer = 0;
	public:
		void Update(const std::shared_ptr<c_Entity>& lPlayer, float partialTick) override;
	};

}

