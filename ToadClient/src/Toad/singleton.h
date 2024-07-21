#pragma once

namespace toadll
{

///
/// Singleton class to inherit from
///
template <class T>
class Singleton
{
	friend T;
public:
	virtual ~Singleton() = default;
	Singleton() = default;
	Singleton(const Singleton&) = delete;

	void operator=(const Singleton&) = delete;

	static T* GetInstance()
	{
		static T instance;
		return &instance;
	}
};

}