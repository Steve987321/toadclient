#pragma once

namespace toadll
{

struct LocalPlayer;

class CVarsUpdater SET_MODULE_CLASS(CVarsUpdater)
{
public:
	/// True when player is in a world and the local player is valid.
	static inline std::atomic_bool IsVerified = false;

	static inline std::shared_ptr<LocalPlayer> theLocalPlayer = std::make_shared<LocalPlayer>();

	/// True when the local player is looking at another player 
	static inline std::atomic_bool IsMouseOverPlayer = false;

	/// The player the local player is looking at.
	///	Only gets updated when IsMouseOverPlayer is true
	static inline Entity MouseOverPlayer {};

	static inline std::atomic<float> PartialTick = 0;
	static inline std::atomic<float> RenderPartialTick = 0;

	/// True when the player has any menu opened
	static inline std::atomic_bool IsInGui = false;

	static inline std::array<float, 16> ModelView = {};
	static inline std::array<float, 16> Projection = {};
	static inline std::array<int, 4> Viewport = {};

public:
	CVarsUpdater() = default;

public:
	void PreUpdate() override;
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;

};

}