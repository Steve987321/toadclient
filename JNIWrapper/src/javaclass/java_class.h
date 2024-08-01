#pragma once

#include "java_field.h"

class JavaClass
{
public:
	JavaClass(jclass klass);
	~JavaClass();

public:
	std::vector<JavaField> fields;
	std::vector<JavaMethod> methods;

protected:
	jclass m_klass;
};

class Player : JavaClass
{

};
class Minecraft : JavaClass
{
public:
	Player GetPlayer()
	{
		env->GetObjectField(m_class, "GetPlayer");
	}
};
