// @otlicense

#pragma once
#include "PropertyBundleDataHandle.h"
#include "PropertiesVis2D3D.h"

class _declspec(dllexport) PropertyBundleDataHandleVis2D3D : public PropertyBundleDataHandle
{
public:
	PropertyBundleDataHandleVis2D3D(EntityBase * thisObject);

	const PropertiesVis2D3D::VisualizationType GetSelectedVisType(void) const { return selectedVisType; }
	const PropertiesVis2D3D::VisualizationComponent GetSelectedVisComp(void) const {return selectedVisComp; }
	const int GetDownsamplingRate(void) const { return downsamplingRate; }

private:
	PropertiesVis2D3D properties;
	PropertiesVis2D3D::VisualizationType selectedVisType;
	PropertiesVis2D3D::VisualizationComponent selectedVisComp;
	int downsamplingRate;


	virtual void LoadCurrentData(EntityBase * thisObject) override;
};
