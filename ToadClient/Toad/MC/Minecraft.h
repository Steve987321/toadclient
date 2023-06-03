#pragma once
namespace toadll
{
	class c_Minecraft
	{
	private:
		jclass mcclass = nullptr;
		jclass gsclass = nullptr; // game settings

		jclass elbclass = nullptr; // entity living base
		jclass ariclass = nullptr; // active render info

	public:
		explicit c_Minecraft(jclass mc_jclass) : mcclass(mc_jclass) {}
		c_Minecraft() = delete;
		~c_Minecraft();
	public:
		_NODISCARD jclass get_mcclass();
		_NODISCARD jclass get_entity_living_class();

		_NODISCARD std::unique_ptr<c_ActiveRenderInfo> get_active_render_info();

		_NODISCARD jobject get_mc() const;
		_NODISCARD jobject get_rendermanager() const;
		_NODISCARD jobject get_localplayerobj() const;
		_NODISCARD jobject get_world() const;
		_NODISCARD jobject get_gamesettings() const;

		_NODISCARD float get_partialTick() const;
		_NODISCARD float get_renderPartialTick() const;
		_NODISCARD float get_fov() const;

		_NODISCARD std::vector<std::shared_ptr<c_Entity>> get_playerList() const;

		_NODISCARD std::shared_ptr<c_Entity> get_localplayer() const;

	public:
		void set_gamma(float val) const;

		//void disableLightMap() const;
		//void enableLightMap() const;
	};

	inline std::unique_ptr<c_Minecraft> p_Minecraft = nullptr;
}
