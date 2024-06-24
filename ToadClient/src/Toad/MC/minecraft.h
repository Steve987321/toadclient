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

private:
	//jclass mcclass = nullptr;
	jclass m_gsclass = nullptr; // game settings

	jclass m_mcclass = nullptr; // minecraft 
	jclass m_elbclass = nullptr; // entity living base
	jclass m_ariclass = nullptr; // active render info
	jclass m_vec3class = nullptr; // Vec3
	jclass m_vec3iclass = nullptr; // Vec3i
	jclass m_mopclass = nullptr; // moving object position 

public:
	explicit Minecraft() = default;
	~Minecraft();

public:
	enum class RAYTRACE_BLOCKS_RESULT
	{
		HIT,					// hit a block
		HIT_FROM_AIRBLOCK,		// hit a block but ray origin was airblock
		NO_HIT,					// the void
		NO_HIT_FROM_AIRBLOCK,	// the void but ray origin was airblock
		ERROR					// debugging purposes
	};

public:
	static jclass findMcClass(JNIEnv* env);

	jclass getMcClass();
	jclass getEntityLivingClass();
	jclass getVec3Class();
	jclass getVec3iClass();
	jclass getMovingObjPosClass();

	std::unique_ptr<ActiveRenderInfo> getActiveRenderInfo();
	//std::unique_ptr<RenderManager> getRenderManager();

	jobject getMc();
	jobject getLocalPlayerObject();
	jobject getWorld();
	jobject getGameSettings();
	jobject getMouseOverObject();
	int getBlockIdAt(const Vec3i& pos);

	bool isInGui();

	std::array<std::string, 27> GetChestContents();

	float getPartialTick();
	float getRenderPartialTick();
	float getFov();

	std::string getMouseOverStr();

	bool isAirBlock(jobject blockobj);

	std::vector<std::shared_ptr<c_Entity>> getPlayerList();

	std::shared_ptr<c_Entity> getMouseOverPlayer();
	std::shared_ptr<c_Entity> getLocalPlayer();

	// TODO: instead of returning a string return an enum of object types
	std::string getMouseOverTypeStr();

	int getLeftClickCounter();

	/// @return a result of the ray trace
	///
	///	@param from beginning of ray
	///	@param direction direction of ray, make sure it is big value
	///	@param result if return value was HIT the block position of the block, else {-1.5 ..}
	///	@param stopOnLiquid whether the ray should count as hit on liquid blocks
	///	@param stopOnAirBlocks whether the ray shouldn't continue if from is an airblock
	RAYTRACE_BLOCKS_RESULT rayTraceBlocks(Vec3 from, Vec3 direction, Vec3& result, bool stopOnLiquid = false, bool stopOnAirBlocks = false, int subtractY = 2);

public:
	std::string movingObjPosToStr(jobject mopObj);

public:
	void set_gamma(float val);
	void setObjMouseOver(jobject newMopObj);
	void setLeftClickCounter(int val);

	//void disableLightMap() const;
	//void enableLightMap() const;
};

}
