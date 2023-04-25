#pragma once
#include "EntityBase.h"
class __declspec(dllexport)  PropertyBundle
{
	virtual void SetProperties(EntityBase * thisObject) = 0;
	virtual bool UpdatePropertyVisibility(EntityBase * thisObject) = 0;
};
