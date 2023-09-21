#pragma once
#include <string>
#include <map>

class _declspec(dllexport) PropertiesVisUnstructuredScalar
{
public:
	enum class VisualizationType
	{
		Isosurface, Points, Contour2D, UNKNOWN
	};

	const std::string GetNameVisType(void) { return nameVisType; }
	const std::string GetNameMaxPoints(void) { return nameMaxPoints; }
	const std::string GetNamePointScale(void) { return namePointScale; }
	const std::string GetNameNumberIsosurfaces(void) { return nameNumberIsosurfaces; }
	const std::string GetNameShow2DMesh(void) { return nameShow2dMesh; }
	const std::string GetName2DMeshColor(void) { return name2dMeshColor; }
	const std::string GetNameShow2DIsolines(void) { return nameShow2dIsolines; }
	const std::string GetName2DIsolineColor(void) { return name2dIsolineColor; }

	const std::string GetValueIsosurface(void) { return valueIsosurface; }
	const std::string GetValuePoints(void) { return valuePoints; }
	const std::string GetValueContour2D(void) { return valueContour2D; }

	const VisualizationType GetVisualizationType(const std::string type) const
	{
		VisualizationType returnVal;
		visualizationTypeMapping.find(type) == visualizationTypeMapping.end() ? returnVal = VisualizationType::UNKNOWN : returnVal = visualizationTypeMapping.at(type);
		return returnVal;
	}

private:
	const std::string nameVisType = "Type";
	const std::string nameMaxPoints = "Max. Points";
	const std::string namePointScale = "Point Scale";
	const std::string nameNumberIsosurfaces = "Number Isosurfaces";

	const std::string valueIsosurface = "Isosurface";
	const std::string valuePoints     = "Points";
	const std::string valueContour2D  = "2D Contour";

	const std::string nameShow2dMesh = "Show 2D Mesh";
	const std::string name2dMeshColor = "2D Mesh Color";
	const std::string nameShow2dIsolines = "Show Isolines";
	const std::string name2dIsolineColor = "Isoline Color";

	std::map<std::string, VisualizationType> visualizationTypeMapping { {valueIsosurface,VisualizationType::Isosurface},{valuePoints,VisualizationType::Points},{valueContour2D,VisualizationType::Contour2D} };
};
