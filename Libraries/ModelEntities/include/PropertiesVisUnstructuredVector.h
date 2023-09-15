#pragma once
#include <string>
#include <map>

class _declspec(dllexport) PropertiesVisUnstructuredVector
{
public:
	enum class VisualizationType
	{
		Arrows3D, Arrows2D, Contour2D, UNKNOWN
	};
	enum class VisualizationComponent
	{
		X,Y,Z,Abs, UNKNOWN
	};
	enum class VisualizationArrowType
	{
		ARROW_FLAT, ARROW_SHADED, HEDGHEHOG, UNKNOWN
	};

	const std::string GetNameVisType(void) { return nameVisType; }
	const std::string GetNameVisComponent(void) { return nameVisComponent; }
	const std::string GetNamePhase(void) { return namePhase; }
	const std::string GetNameTime(void) { return nameTime; }
	const std::string GetNamePlotDownSampling(void) { return namePlotDownSampling; }
	const std::string GetNameArrowType(void) { return nameArrowType; }
	const std::string GetNameArrowScale(void) { return nameArrowScale; }
	const std::string GetNameShow2DMesh(void) { return nameShow2dMesh; }
	const std::string GetName2DMeshColor(void) { return name2dMeshColor; }
	const std::string GetNameShow2DIsolines(void) { return nameShow2dIsolines; }
	const std::string GetName2DIsolineColor(void) { return name2dIsolineColor; }

	const std::string GetValueArrows3D(void) { return valueArrows3D; }
	const std::string GetValueArrows2D(void) { return valueArrows2D; }
	const std::string GetValueContour2D(void) { return valueContour2D; }
	const std::string GetValueComponentX(void) { return valueComponentX; }
	const std::string GetValueComponentY(void) { return valueComponentY; }
	const std::string GetValueComponentZ(void) { return valueComponentZ; }
	const std::string GetValueAbsolute(void) { return valueAbsolute; }
	const std::string GetValueArrowTypeFlat(void) { return arrowTypeFlat; }
	const std::string GetValueArrowTypeShaded(void) { return arrowTypeShaded; }
	const std::string GetValueArrowTypeHedgehog(void) { return arrowTypeHedgehog; }

	const VisualizationType GetVisualizationType(const std::string type) const
	{
		VisualizationType returnVal;
		visualizationTypeMapping.find(type) == visualizationTypeMapping.end() ? returnVal = VisualizationType::UNKNOWN : returnVal = visualizationTypeMapping.at(type);
		return returnVal;
	}

	const VisualizationComponent GetVisualizationComponent(const std::string component)
	{
		VisualizationComponent returnVal;
		visualizationComponentMapping.find(component) == visualizationComponentMapping.end() ? returnVal = VisualizationComponent::UNKNOWN : returnVal = visualizationComponentMapping.at(component);
		return returnVal;
	}

	const VisualizationArrowType GetArrowType(const std::string type)
	{
		VisualizationArrowType returnVal;
		arrowTypeMapping.find(type) == arrowTypeMapping.end() ? returnVal = VisualizationArrowType::UNKNOWN : returnVal = arrowTypeMapping.at(type);
		return returnVal;
	}

private:
	const std::string nameVisType = "Type";
	const std::string nameVisComponent = "Component";
	const std::string namePhase = "Phase";
	const std::string nameTime = "Time";
	const std::string namePlotDownSampling = "Down Sampling";
	const std::string nameArrowType = "Arrow Type";
	const std::string nameArrowScale = "Arrow Scale";

	const std::string valueArrows3D = "3D Arrows";
	const std::string valueArrows2D = "2D Arrows";
	const std::string valueContour2D = "2D Contour";

	const std::string valueComponentX = "X";
	const std::string valueComponentY = "Y";
	const std::string valueComponentZ = "Z";
	const std::string valueAbsolute = "Abs";

	const std::string arrowTypeFlat = "3D Flat";
	const std::string arrowTypeShaded = "3D Shaded";
	const std::string arrowTypeHedgehog = "Hedgehog";

	const std::string nameShow2dMesh = "Show 2D Mesh";
	const std::string name2dMeshColor = "2D Mesh Color";
	const std::string nameShow2dIsolines = "Show Isolines";
	const std::string name2dIsolineColor = "Isoline Color";

	//Not supported yet
	const std::string valueCarpet = "Carpet";
	const std::string valueIsolines = "Isolines";

	std::map<std::string, VisualizationType> visualizationTypeMapping { {valueArrows3D,VisualizationType::Arrows3D},{valueArrows2D,VisualizationType::Arrows2D},{valueContour2D,VisualizationType::Contour2D} };
	std::map < std::string, VisualizationComponent> visualizationComponentMapping{ {valueComponentX,VisualizationComponent::X},{valueComponentY,VisualizationComponent::Y}, {valueComponentZ,VisualizationComponent::Z},{valueAbsolute,VisualizationComponent::Abs} };
	std::map < std::string, VisualizationArrowType> arrowTypeMapping{ {arrowTypeFlat,VisualizationArrowType::ARROW_FLAT},{arrowTypeShaded,VisualizationArrowType::ARROW_SHADED}, {arrowTypeHedgehog,VisualizationArrowType::HEDGHEHOG} };
};
