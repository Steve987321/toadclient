#pragma once

namespace toadll
{
	class c_Entity
	{
	public:
		std::shared_ptr<jobject> obj = nullptr;

	public:
		//explicit c_Entity(const std::shared_ptr<jobject>& jobj) { obj = jobj; }
		explicit c_Entity(std::shared_ptr<jobject> jobj) { obj = jobj; }
		~c_Entity();

	public:
		[[nodiscard]] vec3 get_position() const;
		[[nodiscard]] float get_rotationYaw() const;
		[[nodiscard]] float get_rotationPitch() const;
		[[nodiscard]] std::string get_name() const;
		

	public:
		void set_rotationYaw(float newYaw) const;
		void set_rotationPitch(float newPitch) const;
		void set_rotation(float yaw, float pitch) const;
	};

}
