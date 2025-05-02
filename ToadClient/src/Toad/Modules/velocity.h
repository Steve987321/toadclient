#pragma once

namespace toad
{

	class CVelocity : public CModule
	{
	public:
		using CModule::CModule;

		void PreUpdate() override;
		void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;

	private:
		bool isDirectionAligned(float yaw, float hdirx, float hdirz);
	};

}

