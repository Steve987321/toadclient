#pragma once

namespace toadll
{

/**
 * @brief Singleton class interface
 */
template <class T>
class Singleton
{
public:
	Singleton(Singleton&) = delete;
	Singleton() = default;

public:
	void operator=(const Singleton&) = delete;

	_NODISCARD static T* get_instance()
	{
		static std::shared_ptr<T> instance = std::make_shared<T>();
		return instance.get();
	}
};

}