#pragma once
namespace toadll
{

class CReach SET_MODULE_CLASS(CReach)
{
public:
	void PreUpdate() override;
	void Update(const std::shared_ptr<LocalPlayer>&lPlayer) override;

	static void Invoke(const std::shared_ptr<LocalPlayer>&lPlayer, std::shared_ptr<Minecraft>&MC, JNIEnv * env);
};

}
