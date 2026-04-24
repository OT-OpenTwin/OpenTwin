#include "OTCore/Python/PyhonParameterBuilderValueComparisons.h"

PythonParameter PyhonParameterBuilderValueComparisons::create(const std::string& _entityName, const std::list<ot::ValueComparisonDescription>& _valueComp)
{
	PythonParameter param;
	param.setCallingEntity(_entityName);
	ot::JsonDocument parameterDoc;
	ot::JsonArray valueComparisons;
	for (const auto& valueComp : _valueComp)
	{
		ot::JsonValue entry;
		valueComp.addToJsonObject(entry, parameterDoc.GetAllocator());
		valueComparisons.PushBack(entry, parameterDoc.GetAllocator());
	}
	parameterDoc.AddMember("ValueComparisons", valueComparisons, parameterDoc.GetAllocator());
	param.setAdditionalParameter(std::move(parameterDoc));
	param.setPythonParameterType(getTypeName());
	return param;
}


std::list<ot::ValueComparisonDescription> PyhonParameterBuilderValueComparisons::extract(const PythonParameter& _parameter)
{
	assert(_parameter.getPythonParameterType() == getTypeName());
	std::list<ot::ValueComparisonDescription> valueComparisons;
	const ot::JsonDocument& parameterDoc = _parameter.getAdditionalParameter();
	if (parameterDoc.HasMember("ValueComparisons"))
	{
		ot::ConstJsonArray valueComparisonArray = ot::json::getArray(parameterDoc, "ValueComparisons");
		for (const auto& valueComparisonEntry : valueComparisonArray)
		{
			ot::ValueComparisonDescription valueComp;
			valueComp.setFromJsonObject(valueComparisonEntry.GetObject());
			valueComparisons.push_back(valueComp);
		}
	}
	return valueComparisons;
}
