#pragma once
#include "ClassFactoryHandlerAbstract.h"

class EntityBase;

class _declspec(dllexport) ClassFactory : public ClassFactoryHandlerAbstract
{
public:
	virtual EntityBase* CreateEntity(const std::string& className) override;

	//ClassFactory() = delete;
};

