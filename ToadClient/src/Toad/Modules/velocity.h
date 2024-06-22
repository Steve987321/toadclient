#pragma once

namespace toadll
{

	class CVelocity SET_MODULE_CLASS(CVelocity)
	{
	public:
		void PreUpdate() override;
		void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;

	private:
		bool isDirectionAligned(float yaw, float hdirx, float hdirz);
	};

}

