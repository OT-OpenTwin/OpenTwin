#pragma once

#include "ClassFactoryCAD.h"

class ClassFactoryModel : public ClassFactoryCAD
{
public:
	ClassFactoryModel() {};
	virtual ~ClassFactoryModel() {};

	virtual EntityBase *CreateEntity(const std::string &entityType) override;
};

