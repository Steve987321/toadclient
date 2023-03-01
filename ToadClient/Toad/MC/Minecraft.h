#pragma once
namespace toadll
{
	class c_Minecraft
	{
	private:
		jclass mcclass = nullptr;
		jclass elbclass = nullptr;
		jclass ariclass = nullptr;

	public:
		explicit c_Minecraft(jclass mc_jclass) : mcclass(mc_jclass) {}
		~c_Minecraft();
	public:
		_NODISCARD jclass get_mcclass();

		_NODISCARD jclass get_entity_living_class();

		_NODISCARD std::unique_ptr<c_ActiveRenderInfo> get_active_render_info();

		_NODISCARD jobject get_mc() const;
		_NODISCARD jobject get_rendermanager() const;
		_NODISCARD jobject get_localplayerobj() const;
		_NODISCARD jobject get_world() const;

		_NODISCARD std::vector<std::shared_ptr<c_Entity>> get_playerList() const;

		_NODISCARD std::shared_ptr<c_Entity> get_localplayer() const;

		//void disableLightMap() const;
		//void enableLightMap() const;
	};

	inline std::unique_ptr<c_Minecraft> p_Minecraft = nullptr;
}
