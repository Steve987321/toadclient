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
		_NODISCARD Vec3 getPosition() const;
		_NODISCARD Vec3 getLastTickPosition() const;

		_NODISCARD float getRotationYaw() const;
		_NODISCARD float getRotationPitch() const;

		_NODISCARD jobject getNameObj() const;
		_NODISCARD std::string getName() const;
		_NODISCARD std::string getHeldItemStr() const;
		_NODISCARD jobject getHeldItem() const;

		_NODISCARD std::string getSlotStr(int slot) const;

		_NODISCARD int getHurtTime() const;
		_NODISCARD float getHealth() const;

		_NODISCARD float getMotionX() const;
		_NODISCARD float getMotionY() const;
		_NODISCARD float getMotionZ() const;

		_NODISCARD bool isInvisible() const;
		bool isSneaking() const;

		//[[nodiscard]] jobject get_open_container() const;
		_NODISCARD BBox get_BBox() const;

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