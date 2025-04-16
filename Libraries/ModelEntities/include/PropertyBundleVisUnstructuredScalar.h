#pragma once
#include "PropertyBundle.h"

#include "PropertiesVisUnstructuredScalar.h"

class __declspec(dllexport) PropertyBundleVisUnstructuredScalar : public PropertyBundle
{
public:
	virtual void setProperties(EntityBase * _thisObject) override;
	virtual bool updatePropertyVisibility(EntityBase * _thisObject) override;

	bool is2dType(EntityBase* _thisObject);

private:
	PropertiesVisUnstructuredScalar m_properties;
	const std::string m_defaultCategory = "3D Visualization";
	const std::string m_groupNameGeneral = "General";
	const std::string m_groupNameIsosurfaces = "Isosurfaces";
	const std::string m_groupNamePoints = "Points";
	const std::string m_groupNameContour = "Contour";
};
