#pragma once
#include "PropertyBundle.h"
#include "PlaneProperties.h"

class __declspec(dllexport) PropertyBundlePlane : public PropertyBundle
{
public:
	virtual void SetProperties(EntityBase * thisObject) override;
	virtual bool UpdatePropertyVisibility(EntityBase * thisObject) override;

private:
	PlaneProperties properties;
};
