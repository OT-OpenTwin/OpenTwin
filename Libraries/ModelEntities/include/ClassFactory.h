#pragma once
#include "ClassFactoryHandlerAbstract.h"

class EntityBase;

class _declspec(dllexport) ClassFactory : public ClassFactoryHandlerAbstract
{
public:
	virtual EntityBase* createEntity(const std::string& _className) override;
};

