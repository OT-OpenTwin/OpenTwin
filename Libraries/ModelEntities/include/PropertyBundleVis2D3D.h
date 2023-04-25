#pragma once
#include "PropertyBundle.h"
#include "PropertiesVis2D3D.h"

class __declspec(dllexport) PropertyBundleVis2D3D : public PropertyBundle
{
public:
	virtual void SetProperties(EntityBase * thisObject) override;
	virtual bool UpdatePropertyVisibility(EntityBase * thisObject) override;

private:
	PropertiesVis2D3D properties;
	const std::string defaultCategory = "2D Visualization";
	const std::string groupName = "General";
};
