#pragma once
namespace toadll
{

///
/// Stores important game functions 
///
class Minecraft
{
public:
	JNIEnv* env = nullptr;
	jclass mcclass = nullptr;

private:
	//jclass mcclass = nullptr;
	jclass m_gsclass = nullptr; // game settings

	jclass m_elbclass = nullptr; // entity living base
	jclass m_ariclass = nullptr; // active render info
	jclass m_vec3class = nullptr; // Vec3

public:
	explicit Minecraft() = default;
	~Minecraft();

public:
	_NODISCARD static jclass getMcClass(JNIEnv* env);
	_NODISCARD jclass getEntityLivingClass();
	_NODISCARD jclass getVec3Class();

	_NODISCARD std::unique_ptr<ActiveRenderInfo> getActiveRenderInfo();

	_NODISCARD jobject getMc() const;
	_NODISCARD jobject getRenderManager();
	_NODISCARD jobject getLocalPlayerObject();
	_NODISCARD jobject getWorld();
	_NODISCARD jobject getGameSettings();
	_NODISCARD jobject getMouseOverBlock();
	_NODISCARD int getBlockIdAt(const Vec3& pos);

	_NODISCARD bool isInGui();

	_NODISCARD float getPartialTick();
	_NODISCARD float getRenderPartialTick();
	_NODISCARD float getFov();

	_NODISCARD std::string getMouseOverBlockStr();

	_NODISCARD bool isAirBlock(jobject blockobj);

	_NODISCARD std::vector<std::shared_ptr<c_Entity>> getPlayerList();

	_NODISCARD std::shared_ptr<c_Entity> getMouseOverPlayer();
	_NODISCARD std::shared_ptr<c_Entity> getLocalPlayer();

	/// Returns a block position of the block that has been hit
	///
	///	Could also return {-1.5, ..} if something has gone wrong.
	///	Or could return {-0.5, ..} if nothing has been hit
	Vec3 rayTraceBlocks(Vec3 from, Vec3 direction, bool stopOnLiquid = false);

public:
	void set_gamma(float val);

	//void disableLightMap() const;
	//void enableLightMap() const;
};

}
