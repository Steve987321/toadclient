#pragma once

namespace toadll
{

/**
 * @brief Singleton class to inherit from
 */
template <class T>
class c_Singleton
{
	friend T;
public:
	c_Singleton() = default;
	c_Singleton(const c_Singleton&) = delete;

	void operator=(const c_Singleton&) = delete;

	static T* get_instance()
	{
		static auto instance = std::make_shared<T>();
		return instance.get();
	}
};

}