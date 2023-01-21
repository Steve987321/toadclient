#include "pch.h"
#include "Toad/Toad.h"
#include "Entity.h"

toadll::c_Entity::~c_Entity()
{
	if (*this->obj != nullptr)
		env->DeleteLocalRef(*this->obj);
	std::cout << "deletus" << std::endl;
}

toadll::vec3 toadll::c_Entity::get_position() const
{
	auto posId = get_mid(*obj, mapping::getPos);
	auto position = env->CallObjectMethod(*this->obj, posId);
	return to_vec3(position);
}
