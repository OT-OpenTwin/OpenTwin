#pragma once

#include "ClassFactory.h"

class __declspec(dllexport) ClassFactoryCAD : public ClassFactory
{
public:
	ClassFactoryCAD() {};
	virtual ~ClassFactoryCAD() {};

	virtual EntityBase *CreateEntity(const std::string &entityType) override;
};

