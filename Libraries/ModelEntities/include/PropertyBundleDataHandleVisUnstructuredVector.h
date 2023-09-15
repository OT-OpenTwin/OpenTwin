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
	const PropertiesVisUnstructuredVector::VisualizationArrowType GetSelectedArrowType(void) { return selectedArrowType; }
	const double GetArrowScale(void) { return arrowScale; }
	const bool GetShow2dMesh(void) { return show2dMesh; }
	const bool GetShow2dIsolines(void) { return show2dIsolines; }
	const double GetColor2dMeshR(void) { return color2dMesh[0]; }
	const double GetColor2dMeshG(void) { return color2dMesh[1]; }
	const double GetColor2dMeshB(void) { return color2dMesh[2]; }
	const double GetColor2dIsolinesR(void) { return color2dIsolines[0]; }
	const double GetColor2dIsolinesG(void) { return color2dIsolines[1]; }
	const double GetColor2dIsolinesB(void) { return color2dIsolines[2]; }

private:
	PropertiesVisUnstructuredVector properties;
	PropertiesVisUnstructuredVector::VisualizationType selectedVisType;
	PropertiesVisUnstructuredVector::VisualizationComponent selectedVisComp;
	int downsamplingRate;
	PropertiesVisUnstructuredVector::VisualizationArrowType selectedArrowType;
	double arrowScale;
	bool show2dMesh;
	bool show2dIsolines;
	double color2dMesh[3];
	double color2dIsolines[3];

	virtual void LoadCurrentData(EntityBase * thisObject) override;
};
