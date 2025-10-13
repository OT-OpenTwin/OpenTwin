//#include "stdafx.h"

#include "ClassFactoryCAD.h"

#include "EntityBrep.h"
#include "EntityFaceAnnotation.h"
#include "EntityGeometry.h"
#include "EntityResult3D.h"
#include "EntityUnits.h"

EntityBase *ClassFactoryCAD::createEntity(const std::string &entityType)
{
	if (entityType == "EntityBrep")
	{
		return new EntityBrep(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityFaceAnnotation")
	{
		return new EntityFaceAnnotation(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityGeometry")
	{
		return new EntityGeometry(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityResult3D")
	{
		return new EntityResult3D(0, nullptr, nullptr, nullptr, this, "");
	}
	else if(entityType == "EntityUnits")
	{
		return new EntityUnits(0, nullptr, nullptr, nullptr, this, "");
	}

	return ClassFactoryHandlerAbstract::createEntity(entityType);
}
