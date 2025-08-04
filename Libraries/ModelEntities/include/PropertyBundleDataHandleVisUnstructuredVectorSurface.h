#pragma once
#include "PropertyBundleDataHandle.h"
#include "PropertiesVisUnstructuredVector.h"

class _declspec(dllexport) PropertyBundleDataHandleVisUnstructuredVectorSurface : public PropertyBundleDataHandle
{
public:
	PropertyBundleDataHandleVisUnstructuredVectorSurface(EntityBase * thisObject);

	const int getMaxArrows(void) const { return maxArrows; }
	const PropertiesVisUnstructuredVector::VisualizationArrowType GetSelectedArrowType(void) { return selectedArrowType; }
	const double GetArrowScale(void) { return arrowScale; }


private:
	PropertiesVisUnstructuredVector properties;
	int maxArrows;
	PropertiesVisUnstructuredVector::VisualizationArrowType selectedArrowType;
	double arrowScale;

	virtual void LoadCurrentData(EntityBase * thisObject) override;
};
