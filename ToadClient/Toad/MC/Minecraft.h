#pragma once
namespace toadll
{
	class c_Minecraft
	{
	private:
		jclass mcclass = nullptr;
	public:
		explicit c_Minecraft(jclass mc_jclass) : mcclass(mc_jclass) {}
		~c_Minecraft();
	public:
		[[nodiscard]] jclass get_mcclass() const;
		[[nodiscard]] jobject get_mc() const;
		[[nodiscard]] jobject get_rendermanager() const;
		[[nodiscard]] jobject get_localplayerobj() const;
		[[nodiscard]] jobject get_world() const;
		[[nodiscard]] std::vector<std::shared_ptr<c_Entity>> get_playerList() const;
		[[nodiscard]] std::shared_ptr<c_Entity> get_localplayer() const;

		//void disableLightMap() const;
		//void enableLightMap() const;
	};

	inline std::unique_ptr<c_Minecraft> p_Minecraft = nullptr;
}
