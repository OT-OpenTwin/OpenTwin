#pragma once

#include "ClassFactoryHandlerAbstract.h"

class __declspec(dllexport) ClassFactoryBlock : public ClassFactoryHandlerAbstract
{
public:
	ClassFactoryBlock() {};
	virtual ~ClassFactoryBlock() {};

	virtual EntityBase* createEntity(const std::string& entityType) override;

};

