#pragma once

namespace toadll
{
	class c_Entity
	{
	private:
		JNIEnv* env = nullptr;
		jclass elclass = nullptr;

	public:
		jobject obj = nullptr;

	public:
		explicit c_Entity(jobject jobj, JNIEnv* env, jclass elclass);
		~c_Entity();

	public:
		Vec3 getPosition() const;
		Vec3 getLastTickPosition() const;

		float getRotationYaw() const;
		float getRotationPitch() const;

		jobject getNameObj() const;
		std::string getName() const;
		std::string getHeldItemStr() const;
		jobject getHeldItem() const;

		std::string getSlotStr(int slot) const;

		int getHurtTime() const;
		float getHealth() const;

		float getMotionX() const;
		float getMotionY() const;
		float getMotionZ() const;

		bool isInvisible() const;
		bool isSneaking() const;

		//[[nodiscard]] jobject get_open_container() const;
		BBox get_BBox() const;

		std::array<std::string, 4> getArmor();

	public:
		void setRotationYaw(float newYaw) const;
		void setPrevRotationYaw(float newYaw) const;
		void setRotationPitch(float newPitch) const;
		void setPrevRotationPitch(float newPitch) const;
		void setRotation(float yaw, float pitch) const;
		void setMotionX(float val) const;
		void setMotionY(float val) const;
		void setMotionZ(float val) const;

	};

}
