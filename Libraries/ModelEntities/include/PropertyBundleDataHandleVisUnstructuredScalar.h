#pragma once
#include "PropertyBundleDataHandle.h"
#include "PropertiesVisUnstructuredScalar.h"

class _declspec(dllexport) PropertyBundleDataHandleVisUnstructuredScalar : public PropertyBundleDataHandle
{
public:
	PropertyBundleDataHandleVisUnstructuredScalar(EntityBase * thisObject);

	const PropertiesVisUnstructuredScalar::VisualizationType GetSelectedVisType(void) const { return selectedVisType; }
	const int GetDownsamplingRate(void) const { return downsamplingRate; }
	const double GetPointScale(void) { return pointScale; }
	const int GetNumberIsosurfaces(void) { return numberIsosurfaces; }
	const bool GetShow2dMesh(void) { return show2dMesh; }
	const bool GetShow2dIsolines(void) { return show2dIsolines; }
	const double GetColor2dMeshR(void) { return color2dMesh[0]; }
	const double GetColor2dMeshG(void) { return color2dMesh[1]; }
	const double GetColor2dMeshB(void) { return color2dMesh[2]; }
	const double GetColor2dIsolinesR(void) { return color2dIsolines[0]; }
	const double GetColor2dIsolinesG(void) { return color2dIsolines[1]; }
	const double GetColor2dIsolinesB(void) { return color2dIsolines[2]; }

private:
	PropertiesVisUnstructuredScalar properties;
	PropertiesVisUnstructuredScalar::VisualizationType selectedVisType;
	int downsamplingRate;
	double pointScale;
	int numberIsosurfaces;
	bool show2dMesh;
	bool show2dIsolines;
	double color2dMesh[3];
	double color2dIsolines[3];

	virtual void LoadCurrentData(EntityBase * thisObject) override;
};
