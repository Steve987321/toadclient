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
	jclass m_vec3iclass = nullptr; // Vec3i

public:
	explicit Minecraft() = default;
	~Minecraft();

public:
	enum class RAYTRACE_BLOCKS_RESULT
	{
		HIT,		// hit a block
		HIT_FROM_AIRBLOCK,	// hit a block but ray origin was airblock
		NO_HIT,		// the void
		NO_HIT_FROM_AIRBLOCK,		// the void but ray origin was airblock
		ERROR		// debugging purposes
	};

public:
	_NODISCARD static jclass getMcClass(JNIEnv* env);
	_NODISCARD jclass getEntityLivingClass();
	_NODISCARD jclass getVec3Class();
	_NODISCARD jclass getVec3iClass();

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

	/// @return a result of the ray trace
	///
	///	@param from beginning of ray
	///	@param direction direction of ray, make sure it is big value
	///	@param result if return value was HIT the block position of the block, else {-1.5 ..}
	///	@param stopOnLiquid whether the ray should count as hit on liquid blocks
	///	@param stopOnAirBlocks whether the ray shouldn't continue if from is an airblock
	RAYTRACE_BLOCKS_RESULT rayTraceBlocks(Vec3 from, Vec3 direction, Vec3& result, bool stopOnLiquid = false, bool stopOnAirBlocks = false, int subtractY = 2);

public:
	void set_gamma(float val);

	//void disableLightMap() const;
	//void enableLightMap() const;
};

}
