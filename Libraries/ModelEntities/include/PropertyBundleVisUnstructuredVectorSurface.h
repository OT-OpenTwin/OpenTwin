// @otlicense

#pragma once
#include "PropertyBundle.h"

#include "PropertiesVisUnstructuredVector.h"

class __declspec(dllexport) PropertyBundleVisUnstructuredVectorSurface : public PropertyBundle
{
public:
	virtual void setProperties(EntityBase * _thisObject) override;
	virtual bool updatePropertyVisibility(EntityBase * _thisObject) override;

private:
	PropertiesVisUnstructuredVector m_properties;
	const std::string m_defaultCategory = "3D Visualization";
	const std::string m_groupNameArrows = "Arrows";
};
