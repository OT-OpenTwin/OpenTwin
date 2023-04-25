#include "stdafx.h"

#include "ClassFactoryModel.h"

#include "EntityBrep.h"
#include "EntityFaceAnnotation.h"
#include "EntityGeometry.h"

EntityBase *ClassFactoryModel::CreateEntity(const std::string &entityType)
{
	EntityBase *entity = ClassFactoryCAD::CreateEntity(entityType);
	if (entity != nullptr) return entity;

	assert(0); // Unknown entity
	return nullptr;
}
