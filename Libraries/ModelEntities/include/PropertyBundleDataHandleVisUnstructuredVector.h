#pragma once
#include "PropertyBundleDataHandle.h"
#include "PropertiesVisUnstructuredVector.h"

class _declspec(dllexport) PropertyBundleDataHandleVisUnstructuredVector : public PropertyBundleDataHandle
{
public:
	PropertyBundleDataHandleVisUnstructuredVector(EntityBase * thisObject);

	const PropertiesVisUnstructuredVector::VisualizationType GetSelectedVisType(void) const { return selectedVisType; }
	const PropertiesVisUnstructuredVector::VisualizationComponent GetSelectedVisComp(void) const {return selectedVisComp; }
	const int GetDownsamplingRate(void) const { return downsamplingRate; }

private:
	PropertiesVisUnstructuredVector properties;
	PropertiesVisUnstructuredVector::VisualizationType selectedVisType;
	PropertiesVisUnstructuredVector::VisualizationComponent selectedVisComp;
	int downsamplingRate;


	virtual void LoadCurrentData(EntityBase * thisObject) override;
};
