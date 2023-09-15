#pragma once
#include "PropertyBundle.h"

#include "PropertiesVisUnstructuredVector.h"

class __declspec(dllexport) PropertyBundleVisUnstructuredVector : public PropertyBundle
{
public:
	virtual void SetProperties(EntityBase * thisObject) override;
	virtual bool UpdatePropertyVisibility(EntityBase * thisObject) override;

	bool is2dType(EntityBase* thisObject);

private:
	PropertiesVisUnstructuredVector properties;
	const std::string defaultCategory = "3D Visualization";
	const std::string groupNameGeneral = "General";
	const std::string groupNameArrows = "Arrows";
	const std::string groupNameContour = "Contour";
};
