#pragma once

#include "ClassFactoryHandlerAbstract.h"

class __declspec(dllexport) ClassFactoryBlock : public ClassFactoryHandlerAbstract
{
public:
	ClassFactoryBlock() {};
	virtual ~ClassFactoryBlock() {};

	virtual EntityBase* CreateEntity(const std::string& entityType) override;

};

