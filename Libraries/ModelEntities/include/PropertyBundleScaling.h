#pragma once
#include "PropertyBundle.h"
#include "ScalingProperties.h"

class __declspec(dllexport) PropertyBundleScaling : public PropertyBundle
{
public:
	void setProperties(EntityBase * _thisObject) override;
	bool updatePropertyVisibility(EntityBase * _thisObject) override;
	
private:
	const std::string m_groupName = "Scaling";
	const std::string m_defaultCategory = "Scaling";
	ScalingProperties m_properties;
};
