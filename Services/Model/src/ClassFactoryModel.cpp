#include "stdafx.h"

#include "ClassFactoryModel.h"
#include "EntityBrep.h"
#include "EntityFaceAnnotation.h"
#include "EntityGeometry.h"

ClassFactoryModel::ClassFactoryModel()
{
	_cadEntityHandler = new ClassFactoryCAD();
	_blockEntityHandler = new ClassFactoryBlock();
	_generalEntityHandler = new ClassFactory();

	_cadEntityHandler->SetNextHandler(_blockEntityHandler);
	_blockEntityHandler->SetNextHandler(_generalEntityHandler);
	SetNextHandler(_cadEntityHandler);
}

ClassFactoryModel::~ClassFactoryModel()
{
	if (_cadEntityHandler != nullptr) { delete _cadEntityHandler; _cadEntityHandler = nullptr; }
	if (_blockEntityHandler != nullptr) { delete _blockEntityHandler; _blockEntityHandler = nullptr; }
	if (_generalEntityHandler != nullptr) { delete _generalEntityHandler; _generalEntityHandler = nullptr; }
}

EntityBase *ClassFactoryModel::CreateEntity(const std::string &entityType)
{
	EntityBase* entity = CreateEntity(entityType);
	assert(entity != nullptr);
	return entity;
}
