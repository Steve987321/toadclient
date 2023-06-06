#pragma once
namespace toadll
{
	class c_Minecraft
	{
	public:
		JNIEnv* env = nullptr;
		jclass mcclass = nullptr;

	private:
		//jclass mcclass = nullptr;
		jclass gsclass = nullptr; // game settings

		jclass elbclass = nullptr; // entity living base
		jclass ariclass = nullptr; // active render info

	public:
		explicit c_Minecraft() = default;
		//c_Minecraft() = delete;
		~c_Minecraft();
	public:
		_NODISCARD static jclass get_mcclass(JNIEnv* env);
		_NODISCARD jclass get_entity_living_class();

		_NODISCARD std::unique_ptr<c_ActiveRenderInfo> get_active_render_info();

		_NODISCARD jobject get_mc() const;
		_NODISCARD jobject get_rendermanager() const;
		_NODISCARD jobject get_localplayerobj() const;
		_NODISCARD static jobject get_localplayerobjstatic(JNIEnv* env);
		_NODISCARD jobject get_world() const;
		_NODISCARD jobject get_gamesettings() const;

		_NODISCARD float get_partialTick() const;
		_NODISCARD float get_renderPartialTick() const;
		_NODISCARD float get_fov() const;

		_NODISCARD jobject get_mouseOverObj() const;
		_NODISCARD std::string get_mouseOverStr() const;
		_NODISCARD bool is_AirBlock(jobject blockobj) const;

		_NODISCARD std::vector<std::shared_ptr<c_Entity>> get_playerList();

		_NODISCARD std::shared_ptr<c_Entity> get_localplayer();

	public:
		void set_gamma(float val) const;

		//void disableLightMap() const;
		//void enableLightMap() const;
	};
}
