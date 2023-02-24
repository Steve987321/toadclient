#pragma once

namespace toadll
{
	class c_Entity
	{
	public:
		jobject obj = nullptr;

	public:
		//explicit c_Entity(const std::shared_ptr<jobject>& jobj) { obj = jobj; }
		explicit c_Entity(jobject jobj) { obj = jobj; }
		~c_Entity();

	public:
		[[nodiscard]] vec3 get_position() const;

		[[nodiscard]] float get_rotationYaw() const;
		[[nodiscard]] float get_rotationPitch() const;

		[[nodiscard]] std::string get_name() const;
		[[nodiscard]] jobject get_heldItem() const;

		[[nodiscard]] float get_motionX() const;
		[[nodiscard]] float get_motionY() const;
		[[nodiscard]] float get_motionZ() const;

		[[nodiscard]] bool is_invisible() const;

		//[[nodiscard]] jobject get_open_container() const;

	public:
		void set_rotationYaw(float newYaw) const;
		void set_prevRotationYaw(float newYaw) const;
		void set_rotationPitch(float newPitch) const;
		void set_prevRotationPitch(float newPitch) const;
		void set_rotation(float yaw, float pitch) const;
	};

}
