// @otlicense

#pragma once
#include "EntityBase.h"
class __declspec(dllexport)  PropertyBundle
{
	virtual void setProperties(EntityBase * _thisObject) = 0;
	virtual bool updatePropertyVisibility(EntityBase * _thisObject) = 0;
};
