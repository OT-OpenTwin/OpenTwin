#pragma once

#include "OTCore/QueryDescription/ValueComparisonDescription.h"
#include "OTCore/JSON/JSON.h"
#include <vector>
#include <string>
#include "OTCore/Variable/Variable.h"

class QueryEngine
{
public:
	bool matches(const ot::ValueComparisonDescription& _comparisionDef, const ot::JsonValue& _givenValue);
private:
	bool compareSingleValue(const ot::ValueComparisonDescription& _comparisionDef, const ot::JsonValue& _value);
	struct RangeDefinition
	{
		std::string lowerBound;
		std::string upperBound;
		bool lowerInclusive;
		bool upperInclusive;
	};
	RangeDefinition parseRange(const std::string& _value);
	std::string getJsonValueTypeName(const ot::JsonValue& _value);
	std::vector<std::string> splitCommaSeparated(const std::string& _value);

	bool isInsideRange(
		const ot::Variable& _value,
		const ot::Variable& _lowerBound,
		const ot::Variable& _upperBound,
		bool _lowerInclusive,
		bool _upperInclusive);
	
};
