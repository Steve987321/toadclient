#pragma once

namespace toadll
{
	class c_Entity
	{
	public:
		jobject* obj;

	public:
		explicit c_Entity(jobject* jobj) { obj = jobj; }
		~c_Entity();

	public:
		[[nodiscard]] vec3 get_position() const;

	};

}
