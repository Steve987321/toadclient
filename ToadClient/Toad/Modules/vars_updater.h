#pragma once

namespace toadll
{

struct LocalPlayerT;

class CVarsUpdater SET_MODULE_CLASS(CVarsUpdater)
{
public:
	static inline bool IsVerified = false;
	static inline std::shared_ptr<LocalPlayerT> LocalPlayer = std::make_shared<LocalPlayerT>();
	static inline float PartialTick = 0;
	static inline float RenderPartialTick = 0;
	static inline bool IsInGui = false;
	static inline std::vector<std::shared_ptr<EntityT>> PlayerList = {};

	static inline std::vector<float> ModelView = {};
	static inline std::vector<float> Projection = {};

public:
	void PreUpdate() override;
	void Update(const std::shared_ptr<LocalPlayerT>& lPlayer) override;
};

}