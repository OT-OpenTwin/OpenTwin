#pragma once
#include "PropertyBundle.h"
#include "ScalingProperties.h"

class __declspec(dllexport) PropertyBundleScaling : public PropertyBundle
{
public:
	void SetProperties(EntityBase * thisObject) override;
	bool UpdatePropertyVisibility(EntityBase * thisObject) override;
	
private:
	const std::string groupName = "Scaling";
	const std::string defaultCategory = "Scaling";
	ScalingProperties properties;
};
