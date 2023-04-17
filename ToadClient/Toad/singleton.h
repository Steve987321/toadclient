#pragma once

namespace toadll
{

/**
 * @brief Singleton class interface
 */
template <class T>
class c_Singleton
{
	friend T;
public:
	c_Singleton() = default;
	c_Singleton(const c_Singleton&) = delete;

	void operator=(const c_Singleton&) = delete;

	_NODISCARD static T* get_instance()
	{
		static std::shared_ptr<T> instance = std::make_shared<T>();
		return instance.get();
	}
};

}