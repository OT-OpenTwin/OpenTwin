// @otlicense

#pragma once
#include "PropertyBundle.h"
#include "PlaneProperties.h"

class __declspec(dllexport) PropertyBundlePlane : public PropertyBundle
{
public:
	virtual void setProperties(EntityBase * _thisObject) override;
	virtual bool updatePropertyVisibility(EntityBase * _thisObject) override;
	
	bool hidePlane(EntityBase* _thisObject);

private:
	PlaneProperties m_properties;
};
