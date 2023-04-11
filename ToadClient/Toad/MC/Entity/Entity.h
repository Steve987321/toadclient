#pragma once

namespace toadll
{
	class c_Entity
	{
	public:
		jobject obj = nullptr;

	public:
		explicit c_Entity(jobject jobj) { obj = jobj; }
		~c_Entity();

	public:
		_NODISCARD vec3 get_position() const;

		_NODISCARD float get_rotationYaw() const;
		_NODISCARD float get_rotationPitch() const;

		_NODISCARD std::string get_name() const;
		_NODISCARD std::string get_heldItemStr() const;
		_NODISCARD jobject get_heldItem() const;

		_NODISCARD int get_hurt_time() const;

		_NODISCARD float get_motionX() const;
		_NODISCARD float get_motionY() const;
		_NODISCARD float get_motionZ() const;

		_NODISCARD bool is_invisible() const;

		//[[nodiscard]] jobject get_open_container() const;

	public:
		void set_rotationYaw(float newYaw) const;
		void set_prevRotationYaw(float newYaw) const;
		void set_rotationPitch(float newPitch) const;
		void set_prevRotationPitch(float newPitch) const;
		void set_rotation(float yaw, float pitch) const;
		void set_motionX(float val);
		void set_motionY(float val);
		void set_motionZ(float val);

	};

}
