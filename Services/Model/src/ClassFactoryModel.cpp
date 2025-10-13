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
	_cadEntityHandler->setChainRoot(_generalEntityHandler);
	_blockEntityHandler->setChainRoot(_generalEntityHandler);

	_generalEntityHandler->setNextHandler(_cadEntityHandler);
	_cadEntityHandler->setNextHandler(_blockEntityHandler);
	setNextHandler(_generalEntityHandler);
}

ClassFactoryModel::~ClassFactoryModel()
{
	if (_cadEntityHandler != nullptr) { delete _cadEntityHandler; _cadEntityHandler = nullptr; }
	if (_blockEntityHandler != nullptr) { delete _blockEntityHandler; _blockEntityHandler = nullptr; }
	if (_generalEntityHandler != nullptr) { delete _generalEntityHandler; _generalEntityHandler = nullptr; }
}
