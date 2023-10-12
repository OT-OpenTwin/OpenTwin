#pragma once

#include "ClassFactory.h"
#include "ClassFactoryCAD.h"
#include "ClassFactoryBlock.h"
#include "ClassFactoryHandlerAbstract.h"

class ClassFactoryModel : public ClassFactoryHandlerAbstract
{
public:
	ClassFactoryModel();
	virtual ~ClassFactoryModel();

	virtual EntityBase *CreateEntity(const std::string &entityType) override;

private:
	ClassFactoryCAD* _cadEntityHandler = nullptr;
	ClassFactoryBlock* _blockEntityHandler = nullptr;
	ClassFactory* _generalEntityHandler = nullptr;
};

