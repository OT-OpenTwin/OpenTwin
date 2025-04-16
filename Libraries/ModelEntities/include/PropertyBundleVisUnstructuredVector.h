#pragma once
#include "PropertyBundle.h"

#include "PropertiesVisUnstructuredVector.h"

class __declspec(dllexport) PropertyBundleVisUnstructuredVector : public PropertyBundle
{
public:
	virtual void setProperties(EntityBase * _thisObject) override;
	virtual bool updatePropertyVisibility(EntityBase * _thisObject) override;

	bool is2dType(EntityBase* thisObject);

private:
	PropertiesVisUnstructuredVector m_properties;
	const std::string m_defaultCategory = "3D Visualization";
	const std::string m_groupNameGeneral = "General";
	const std::string m_groupNameArrows = "Arrows";
	const std::string m_groupNameContour = "Contour";
};
