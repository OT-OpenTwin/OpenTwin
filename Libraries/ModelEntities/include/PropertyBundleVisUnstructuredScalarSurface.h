#pragma once
#include "PropertyBundle.h"

#include "PropertiesVisUnstructuredScalarSurface.h"

class __declspec(dllexport) PropertyBundleVisUnstructuredScalarSurface : public PropertyBundle
{
public:
	virtual void SetProperties(EntityBase * thisObject) override;
	virtual bool UpdatePropertyVisibility(EntityBase * thisObject) override;

private:
	PropertiesVisUnstructuredScalarSurface properties;
	const std::string defaultCategory = "3D Visualization";
	const std::string groupNameGeneral = "General";
	const std::string groupNamePoints = "Points";
	const std::string groupNameContour = "Contour";
};
