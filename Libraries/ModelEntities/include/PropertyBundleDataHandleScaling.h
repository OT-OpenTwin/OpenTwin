#pragma once
#include "PropertyBundleDataHandle.h"
#include "ScalingProperties.h"

class __declspec(dllexport) PropertyBundleDataHandleScaling : public PropertyBundleDataHandle
{
public:
	PropertyBundleDataHandleScaling(EntityBase * thisObject);

	const double GetRangeMin(void) const { return rangeMin; }
	const double GetRangeMax(void) const { return rangeMax; }
	const int GetColourResolution(void) const { return colourResolution; }
	const ScalingProperties::ScalingMethod GetScalingMethod(void) { return scalingMethod; }
	const ScalingProperties::ScalingFunction GetScalingFunction(void) { return scalingFunction; }

	bool UpdateMinMaxProperties(double minValue, double maxValue);
	void GetEntityProperties(long long& _UID, long long& Version);

private:
	EntityBase* thisObject = nullptr;

	ScalingProperties properties;
	ScalingProperties::ScalingFunction scalingFunction;
	ScalingProperties::ScalingMethod scalingMethod;
	double rangeMin=-1.;
	double rangeMax=-1.;
	int colourResolution;

	virtual void LoadCurrentData(EntityBase * thisObject) override;
};
