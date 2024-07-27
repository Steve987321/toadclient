#pragma once

template<typename T> 
class JavaField
{
public:
	JavaField(const JavaClass& klass)
public:
	T Get();

private:
	jfieldID id;

public:
	void operator=(const T& other)
	{

	}

	bool operator==(const T& other)
	{

	}

};

