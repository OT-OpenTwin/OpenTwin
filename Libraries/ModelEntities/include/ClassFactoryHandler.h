#pragma once
#pragma warning(disable : 4251)
#include <string>
#include "EntityBase.h"
class _declspec(dllexport) ClassFactoryHandler
{
public:
	virtual ClassFactoryHandler* SetNextHandler(ClassFactoryHandler* nextHandler) = 0;
	virtual EntityBase* CreateEntity(const std::string& className) = 0;
};
