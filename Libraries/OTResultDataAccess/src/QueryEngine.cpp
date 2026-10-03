#include "OTResultDataAccess/JsonQueries/QueryEngine.h"
#include "OTCore/TypeNames.h"
#include "OTCore/Variable/ExplicitStringValueConverter.h"
#include "OTCore/Variable/JSONToVariableConverter.h"
#include "OTCore/String.h"

bool QueryEngine::matches(const ot::ValueComparisonDescription& _comparisonDef, const ot::JsonValue& _givenValue)
{
	const std::string type = getJsonValueTypeName(_givenValue);
	const std::string& comparator = _comparisonDef.getComparator();
	const std::string& queryValue = _comparisonDef.getValue();

	ot::ExplicitStringValueConverter stringConverter;
	ot::JSONToVariableConverter jsonConverter;

	const ot::Variable actualValue = jsonConverter(_givenValue);

	if (comparator == "=")
	{
		const ot::Variable givenValue =
			stringConverter.setValueFromString(queryValue, type);

		return actualValue == givenValue;
	}
	else if (comparator == "!=")
	{
		const ot::Variable givenValue =
			stringConverter.setValueFromString(queryValue, type);

		return actualValue != givenValue;
	}
	else if (comparator == "<")
	{
		const ot::Variable givenValue =
			stringConverter.setValueFromString(queryValue, type);

		return actualValue < givenValue;
	}
	else if (comparator == "<=")
	{
		const ot::Variable givenValue =
			stringConverter.setValueFromString(queryValue, type);

		return actualValue < givenValue || actualValue == givenValue;
	}
	else if (comparator == ">")
	{
		const ot::Variable givenValue =
			stringConverter.setValueFromString(queryValue, type);

		return actualValue > givenValue;
	}
	else if (comparator == ">=")
	{
		const ot::Variable givenValue =
			stringConverter.setValueFromString(queryValue, type);

		return actualValue > givenValue || actualValue == givenValue;
	}
	else if (comparator == "Any of")
	{
		const std::vector<std::string> values =
			splitCommaSeparated(queryValue);

		for (const std::string& textValue : values)
		{
			const ot::Variable candidate =
				stringConverter.setValueFromString(textValue, type);

			if (actualValue == candidate)
			{
				return true;
			}
		}

		return false;
	}
	else if (comparator == "Not any of")
	{
		const std::vector<std::string> values =
			splitCommaSeparated(queryValue);

		for (const std::string& textValue : values)
		{
			const ot::Variable candidate =
				stringConverter.setValueFromString(textValue, type);

			if (actualValue == candidate)
			{
				return false;
			}
		}

		return true;
	}
	else if (comparator == "Range")
	{
		const RangeDefinition range = parseRange(queryValue);

		const ot::Variable lowerBound =
			stringConverter.setValueFromString(range.lowerBound, type);

		const ot::Variable upperBound =
			stringConverter.setValueFromString(range.upperBound, type);

		if (upperBound < lowerBound)
		{
			throw std::invalid_argument(
				"Invalid range: upper bound must not be smaller than lower bound.");
		}

		return isInsideRange(
			actualValue,
			lowerBound,
			upperBound,
			range.lowerInclusive,
			range.upperInclusive);
	}

	throw std::invalid_argument(
		"Unsupported operator for comparison: " + comparator);
}

bool QueryEngine::compareSingleValue(const ot::ValueComparisonDescription& _comparisonDef, const ot::JsonValue& _value)
{
	std::string type;
	if (_value.IsInt())
	{
		type = ot::TypeNames::getInt32TypeName();
	}
	else if (_value.IsInt64())
	{
		type = ot::TypeNames::getInt64TypeName();
	}
	else if (_value.IsFloat())
	{
		type = ot::TypeNames::getFloatTypeName();
	}
	else if (_value.IsDouble())
	{
		type = ot::TypeNames::getDoubleTypeName();
	}
	else if (_value.IsBool())
	{
		type = ot::TypeNames::getBoolTypeName();
	}
	else
	{
		throw std::exception("Not supported data type for comparison");
	}

	ot::ExplicitStringValueConverter converter;
	ot::Variable givenValue = converter.setValueFromString(_comparisonDef.getValue(), type);

	ot::JSONToVariableConverter converterJ;
	ot::Variable isValue = converterJ(_value);

	if (_comparisonDef.getComparator() == "=")
	{
		return givenValue == isValue;
	}
	else if (_comparisonDef.getComparator() == "<")
	{
		return givenValue < isValue;
	}
	else if (_comparisonDef.getComparator() == "<=")
	{
		return (givenValue < isValue) || (givenValue == isValue);
	}
	else if (_comparisonDef.getComparator() == ">")
	{
		return (givenValue > isValue);
	}
	else if (_comparisonDef.getComparator() == ">=")
	{
		return (givenValue > isValue) || (givenValue == isValue);
	}
	else if (_comparisonDef.getComparator() == "!=")
	{
		return !(givenValue == isValue);
	}
	else
	{
		throw std::exception(("Not supported operator for comparison: " + _comparisonDef.getComparator()).c_str());
	}
	return false;
}

QueryEngine::RangeDefinition QueryEngine::parseRange(const std::string& _value)
{
	
	const std::string range = ot::String::trim(_value);

	if (range.size() < 5)
	{
		throw std::invalid_argument(
			"Invalid range. Expected e.g. '[1,3.5)'.");
	}

	const char lowerDelimiter = range.front();
	const char upperDelimiter = range.back();

	if ((lowerDelimiter != '[' && lowerDelimiter != '(') ||
		(upperDelimiter != ']' && upperDelimiter != ')'))
	{
		throw std::invalid_argument(
			"Invalid range delimiters. Expected '[', '(' and ']', ')'.");
	}

	const std::string contents = range.substr(1, range.size() - 2);
	const size_t commaPos = contents.find(',');

	if (commaPos == std::string::npos ||
		contents.find(',', commaPos + 1) != std::string::npos)
	{
		throw std::invalid_argument(
			"Invalid range. Expected exactly two bounds, e.g. '[1,3.5)'.");
	}

	RangeDefinition result;
	result.lowerBound = ot::String::trim(contents.substr(0, commaPos));
	result.upperBound = ot::String::trim(contents.substr(commaPos + 1));
	result.lowerInclusive = lowerDelimiter == '[';
	result.upperInclusive = upperDelimiter == ']';

	if (result.lowerBound.empty() || result.upperBound.empty())
	{
		throw std::invalid_argument(
			"Range bounds must not be empty.");
	}

	return result;
}

std::string QueryEngine::getJsonValueTypeName(const ot::JsonValue& value)
{
	if (value.IsInt())
	{
		return ot::TypeNames::getInt32TypeName();
	}
	else if (value.IsInt64())
	{
		return ot::TypeNames::getInt64TypeName();
	}
	else if (value.IsFloat())
	{
		return ot::TypeNames::getFloatTypeName();
	}
	else if (value.IsDouble())
	{
		return ot::TypeNames::getDoubleTypeName();
	}
	else if (value.IsBool())
	{
		return ot::TypeNames::getBoolTypeName();
	}
	else if (value.IsString())
	{
		return ot::TypeNames::getStringTypeName();
	}
	else
	{
		throw std::invalid_argument("Unsupported data type for comparison.");
	}
}

std::vector<std::string> QueryEngine::splitCommaSeparated(const std::string& _value)
{
	std::vector<std::string> result;
	std::stringstream stream(_value);
	std::string token;

	while (std::getline(stream, token, ','))
	{
		token = ot::String::trim(token);

		if (token.empty())
		{
			throw std::invalid_argument(
				"Comma-separated comparison values must not be empty.");
		}

		result.push_back(std::move(token));
	}

	if (result.empty())
	{
		throw std::invalid_argument(
			"Comma-separated comparison requires at least one value.");
	}

	return result;
}

bool QueryEngine::isInsideRange(const ot::Variable& _value, const ot::Variable& _lowerBound, const ot::Variable& _upperBound, bool _lowerInclusive, bool _upperInclusive)
{
	const bool lowerMatches =
		_lowerInclusive ? _value >= _lowerBound : _value > _lowerBound;

	const bool upperMatches =
		_upperInclusive ? _value <= _upperBound : _value < _upperBound;

	return lowerMatches && upperMatches;
}
