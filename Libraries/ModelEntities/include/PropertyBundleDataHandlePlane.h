// @otlicense

#pragma once
#include "PropertyBundleDataHandle.h"
#include "PlaneProperties.h"

class __declspec(dllexport) PropertyBundleDataHandlePlane : public PropertyBundleDataHandle
{
public:
	explicit PropertyBundleDataHandlePlane(EntityBase * thisObject);

	const double GetNormalValueX(void) const { return normalValueX; }
	const double GetNormalValueY(void) const { return normalValueY; }
	const double GetNormalValueZ(void) const { return normalValueZ; }
	const double GetCenterValueX(void) const { return centerX; }
	const double GetCenterValueY(void) const { return centerY; }
	const double GetCenterValueZ(void) const { return centerZ; }

	const PlaneProperties::NormalDescription GetNormalDescription(void)const { return normalDescription; }

private:
	double normalValueX;
	double normalValueY;
	double normalValueZ;
	double centerX;
	double centerY;
	double centerZ;
	PlaneProperties::NormalDescription normalDescription;
	
	virtual void LoadCurrentData(EntityBase * thisObject) override;
};
