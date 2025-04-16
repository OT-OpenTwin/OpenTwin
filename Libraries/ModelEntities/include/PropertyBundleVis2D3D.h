#pragma once
#include "PropertyBundle.h"
#include "PropertiesVis2D3D.h"

class __declspec(dllexport) PropertyBundleVis2D3D : public PropertyBundle
{
public:
	virtual void setProperties(EntityBase * _thisObject) override;
	virtual bool updatePropertyVisibility(EntityBase * _thisObject) override;

private:
	PropertiesVis2D3D m_properties;
	const std::string m_defaultCategory = "2D Visualization";
	const std::string m_groupName = "General";
};
