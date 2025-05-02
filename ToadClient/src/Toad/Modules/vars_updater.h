#pragma once

#include <array>
#include <mutex>

namespace toad
{

struct LocalPlayer;

class CVarsUpdater : public CModule
{
public:
	using CModule::CModule;

	/// True when player is in a world and the local player is valid.
	static inline bool IsVerified = false;

	static inline std::condition_variable IsVerifiedCV;

	static inline std::shared_ptr<LocalPlayer> theLocalPlayer = std::make_shared<LocalPlayer>();

	static inline std::atomic<float> PartialTick = 0;
	static inline std::atomic<float> RenderPartialTick = 0;

	/// True when the player has any menu opened
	static inline std::atomic_bool IsInGui = false;

	static inline std::array<double, 16> ModelView = {};
	static inline std::array<double, 16> Projection = {};
	static inline std::array<int, 4> Viewport = {};

public:
	void PreUpdate() override;
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;

};

}