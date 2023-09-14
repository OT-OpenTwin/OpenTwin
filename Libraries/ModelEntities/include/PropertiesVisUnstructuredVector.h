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

	const std::string GetNameVisType(void) { return nameVisType; }
	const std::string GetNameVisComponent(void) { return nameVisComponent; }
	const std::string GetNamePhase(void) { return namePhase; }
	const std::string GetNameTime(void) { return nameTime; }
	const std::string GetNamePlotDownSampling(void) { return namePlotDownSampling; }

	const std::string GetValueArrows3D(void) { return valueArrows3D; }
	const std::string GetValueArrows2D(void) { return valueArrows2D; }
	const std::string GetValueContour2D(void) { return valueContour2D; }
	const std::string GetValueComponentX(void) { return valueComponentX; }
	const std::string GetValueComponentY(void) { return valueComponentY; }
	const std::string GetValueComponentZ(void) { return valueComponentZ; }
	const std::string GetValueAbsolute(void) { return valueAbsolute; }

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

private:
	const std::string nameVisType = "Type";
	const std::string nameVisComponent = "Component";
	const std::string namePhase = "Phase";
	const std::string nameTime = "Time";
	const std::string namePlotDownSampling = "Down Sampling";

	const std::string valueArrows3D = "3D Arrows";
	const std::string valueArrows2D = "2D Arrows";
	const std::string valueContour2D = "2D Contour";

	const std::string valueComponentX = "X";
	const std::string valueComponentY = "Y";
	const std::string valueComponentZ = "Z";
	const std::string valueAbsolute = "Abs";

	//Not supported yet
	const std::string valueCarpet = "Carpet";
	const std::string valueIsolines = "Isolines";

	std::map<std::string, VisualizationType> visualizationTypeMapping { {valueArrows3D,VisualizationType::Arrows3D},{valueArrows2D,VisualizationType::Arrows2D},{valueContour2D,VisualizationType::Contour2D} };
	std::map < std::string, VisualizationComponent> visualizationComponentMapping{ {valueComponentX,VisualizationComponent::X},{valueComponentY,VisualizationComponent::Y}, {valueComponentZ,VisualizationComponent::Z},{valueAbsolute,VisualizationComponent::Abs} };
};
