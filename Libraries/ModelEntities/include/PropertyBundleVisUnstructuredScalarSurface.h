#pragma once
#include "PropertyBundle.h"

#include "PropertiesVisUnstructuredScalarSurface.h"

class __declspec(dllexport) PropertyBundleVisUnstructuredScalarSurface : public PropertyBundle
{
public:
	virtual void setProperties(EntityBase * _thisObject) override;
	virtual bool updatePropertyVisibility(EntityBase * _thisObject) override;

private:
	PropertiesVisUnstructuredScalarSurface m_properties;
	const std::string m_defaultCategory = "3D Visualization";
	const std::string m_groupNameGeneral = "General";
	const std::string m_groupNamePoints = "Points";
	const std::string m_groupNameContour = "Contour";
};
