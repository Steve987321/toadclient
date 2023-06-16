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
		_NODISCARD static jclass getMcClass(JNIEnv* env);
		_NODISCARD jclass getEntityLivingClass();

		_NODISCARD std::unique_ptr<c_ActiveRenderInfo> getActiveRenderInfo();

		_NODISCARD jobject getMc();
		_NODISCARD jobject getRenderManager();
		_NODISCARD jobject getLocalPlayerObject();
		_NODISCARD jobject getWorld();
		_NODISCARD jobject getGameSettings();
		_NODISCARD jobject getMouseOverBlock();
		_NODISCARD int getBlockIdAt(const vec3& pos);

		_NODISCARD bool isInGui();

		_NODISCARD float getPartialTick();
		_NODISCARD float getRenderPartialTick();
		_NODISCARD float getFov();

		_NODISCARD std::string getMouseOverBlockStr();

		_NODISCARD bool isAirBlock(jobject blockobj);

		_NODISCARD std::vector<std::shared_ptr<c_Entity>> getPlayerList();

		_NODISCARD std::shared_ptr<c_Entity> getMouseOverPlayer();
		_NODISCARD std::shared_ptr<c_Entity> getLocalPlayer();

	public:
		void set_gamma(float val);

		//void disableLightMap() const;
		//void enableLightMap() const;
	};
}
