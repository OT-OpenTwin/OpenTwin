// @otlicense

#pragma once
#include "EntityBase.h"
class __declspec(dllexport) PropertyBundleDataHandle
{
	virtual void LoadCurrentData(EntityBase * thisObject) = 0;
};
