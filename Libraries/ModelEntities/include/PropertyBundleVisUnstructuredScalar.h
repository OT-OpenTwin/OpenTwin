#pragma once
#include "PropertyBundle.h"

#include "PropertiesVisUnstructuredScalar.h"

class __declspec(dllexport) PropertyBundleVisUnstructuredScalar : public PropertyBundle
{
public:
	virtual void SetProperties(EntityBase * thisObject) override;
	virtual bool UpdatePropertyVisibility(EntityBase * thisObject) override;

	bool is2dType(EntityBase* thisObject);

private:
	PropertiesVisUnstructuredScalar properties;
	const std::string defaultCategory = "3D Visualization";
	const std::string groupNameGeneral = "General";
	const std::string groupNameIsosurfaces = "Isosurfaces";
	const std::string groupNamePoints = "Points";
	const std::string groupNameContour = "Contour";
};
