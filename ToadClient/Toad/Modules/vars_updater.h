#pragma once

namespace toadll
{

struct LocalPlayerT;

class CVarsUpdater SET_MODULE_CLASS(CVarsUpdater)
{
public:
	static inline std::atomic_bool IsVerified = false;
	static inline std::shared_ptr<LocalPlayerT> LocalPlayer = std::make_shared<LocalPlayerT>();
	static inline std::atomic_bool IsMouseOverPlayer = false;
	static inline EntityT MouseOverPlayer;
	static inline std::atomic<float> PartialTick = 0;
	static inline std::atomic<float> RenderPartialTick = 0;
	static inline std::atomic_bool IsInGui = false;

	static inline std::vector<float> ModelView = {};
	static inline std::vector<float> Projection = {};
	//static inline std::vector<std::shared_ptr<EntityT>> PlayerList = {};

private:
	static inline std::vector<EntityT> m_playerList = {};

public:
	std::shared_mutex PlayerListMutex;
	static std::vector<EntityT> GetPlayerList() { return m_playerList; }

public:
	CVarsUpdater() = default;

public:
	void PreUpdate() override;
	void Update(const std::shared_ptr<LocalPlayerT>& lPlayer) override;
};

}