#include "stdafx.h"

#include "ClassFactoryModel.h"
#include "EntityBrep.h"
#include "EntityFaceAnnotation.h"
#include "EntityGeometry.h"

ClassFactoryModel::ClassFactoryModel()
{
	_generalEntityHandler = new ClassFactory();
	_cadEntityHandler = new ClassFactoryCAD();
	_blockEntityHandler = new ClassFactoryBlock();
	_cadEntityHandler->SetChainRoot(_generalEntityHandler);
	_blockEntityHandler->SetChainRoot(_generalEntityHandler);

	_generalEntityHandler->SetNextHandler(_cadEntityHandler);
	_cadEntityHandler->SetNextHandler(_blockEntityHandler);
	SetNextHandler(_generalEntityHandler);
}

ClassFactoryModel::~ClassFactoryModel()
{
	if (_cadEntityHandler != nullptr) { delete _cadEntityHandler; _cadEntityHandler = nullptr; }
	if (_blockEntityHandler != nullptr) { delete _blockEntityHandler; _blockEntityHandler = nullptr; }
	if (_generalEntityHandler != nullptr) { delete _generalEntityHandler; _generalEntityHandler = nullptr; }
}
