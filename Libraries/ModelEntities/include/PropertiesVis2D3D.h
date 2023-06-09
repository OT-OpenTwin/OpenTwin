#pragma once
#include <string>
#include <map>

class _declspec(dllexport) PropertiesVis2D3D
{
public:
	enum class VisualizationType
	{
		Arrows, Contour, UNKNOWN
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

	const std::string GetValueArrows(void) { return valueArrows; }
	const std::string GetValueContour(void) { return valueContour; }
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

	const std::string valueArrows = "Arrows";
	const std::string valueContour = "Contour";

	const std::string valueComponentX = "X";
	const std::string valueComponentY = "Y";
	const std::string valueComponentZ = "Z";
	const std::string valueAbsolute = "Abs";

	//Not supported yet
	const std::string valueCarpet = "Carpet";
	const std::string valueIsolines = "Isolines";

	std::map<std::string, VisualizationType> visualizationTypeMapping { {valueArrows,VisualizationType::Arrows},{valueContour,VisualizationType::Contour} };
	std::map < std::string, VisualizationComponent> visualizationComponentMapping{ {valueComponentX,VisualizationComponent::X},{valueComponentY,VisualizationComponent::Y}, {valueComponentZ,VisualizationComponent::Z},{valueAbsolute,VisualizationComponent::Abs} };
};
