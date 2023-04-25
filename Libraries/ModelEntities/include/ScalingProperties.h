#pragma once
#include <string>
#include <map>

class __declspec(dllexport) ScalingProperties
{
public:
	enum class ScalingMethod
	{
		autoScale, rangeScale, UNKNOWN
	};
	enum class ScalingFunction
	{
		linScale, logScale, UNKNOWN
	};

	const std::string GetPropertyNameScalingMethod(void) { return nameScalingMethod; }
	const std::string GetPropertyNameRangeMax(void) { return nameRangeMax; }
	const std::string GetPropertyNameRangeMin(void) { return nameRangeMin; }
	const std::string GetPropertyNameScalingFunction(void) { return nameScalingFunction; }
	const std::string GetPropertyNameColourResolution(void) { return nameColourResolution; }

	const std::string GetPropertyValueAutoscaling(void) { return valueAutoScaling; }
	const std::string GetPropertyValueRangeScaling(void) { return valueRangeScaling; }
	const std::string GetPropertyValueLogscaling(void) { return valueLogScale; }
	const std::string GetPropertyValueLinScaling(void) { return valueLinScale; }

	const ScalingMethod GetScalingMethod(std::string scaling)
	{
		ScalingMethod returnVal;
		scalingMethodMapping.find(scaling) == scalingMethodMapping.end() ? returnVal = ScalingMethod::UNKNOWN : returnVal = scalingMethodMapping.at(scaling);
		return returnVal;
	}

	const ScalingFunction GetScalingFunction(std::string funct)
	{
		ScalingFunction returnVal;
		scalingFunctionMapping.find(funct) == scalingFunctionMapping.end() ? returnVal = ScalingFunction::UNKNOWN : returnVal = scalingFunctionMapping.at(funct);
		return returnVal;
	}

private:
	const std::string nameScalingMethod = "Scaling Method";
	const std::string valueRangeScaling = "Range";
	const std::string nameRangeMax = "Max";
	const std::string nameRangeMin = "Min";
	const std::string valueAutoScaling = "Auto";

	const std::string nameScalingFunction = "Scaling Function";
	const std::string valueLogScale = "Log10 scale";
	const std::string valueLinScale = "Linear scale";

	const std::string nameColourResolution = "Colour Resolution";

	std::map < std::string, ScalingMethod> scalingMethodMapping{ {valueAutoScaling, ScalingMethod::autoScale}, {valueRangeScaling, ScalingMethod::rangeScale} };
	std::map < std::string, ScalingFunction> scalingFunctionMapping{ {valueLinScale,ScalingFunction::linScale}, {valueLogScale, ScalingFunction::logScale} };
};
