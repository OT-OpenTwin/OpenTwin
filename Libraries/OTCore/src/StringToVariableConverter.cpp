// @otlicense

#include "OTCore/StringToVariableConverter.h"
#include "OTCore/Json.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/StringToNumericCheck.h"

ot::Variable ot::StringToVariableConverter::operator()(const std::string& _value, const char _decimalSeparator)
{
	char thousandsSeparator(' ');
	bool needsNormalisation = false;
	if (_decimalSeparator == '.')
	{
		thousandsSeparator = ',';
	}
	else
	{
		thousandsSeparator = '.';
		needsNormalisation = true;
	}
	size_t numberOfDecimalSeparators = std::count(_value.begin(), _value.end(), _decimalSeparator);
	bool hasInvertedCommas = _value.find('"') != std::string::npos;

	ot::Variable transformedValue;
	if (numberOfDecimalSeparators <= 1 && !hasInvertedCommas)
	{
		std::string value = _value;
		removeThousandsSeparators(value, thousandsSeparator);
		removeWhitespaces(value);
		if (needsNormalisation)
		{
			ensurePointAsDecimalSeparator(value);
		}
		transformedValue = convertIfFitsNumericalValue(value);
	}
	else
	{
		transformedValue = ot::Variable(_value);
	}
	return transformedValue;
}

bool ot::StringToVariableConverter::normaliseNumericString(std::string& _value, const char _decimalSeparator)
{
	char thousandsSeparator(' ');
	bool needsNormalisation = false;
	if (_decimalSeparator == '.')
	{
		thousandsSeparator = ',';
	}
	else
	{
		thousandsSeparator = '.';
		needsNormalisation = true;
	}
	size_t numberOfDecimalSeparators = std::count(_value.begin(), _value.end(), _decimalSeparator);
	_value.erase(std::remove(_value.begin(), _value.end(), '"'), _value.end());
	if (numberOfDecimalSeparators <= 1)
	{
		removeThousandsSeparators(_value, thousandsSeparator);
		removeWhitespaces(_value);
		removeControlCharacters(_value);
		if (needsNormalisation)
		{
			ensurePointAsDecimalSeparator(_value);
		}
		return true;
	}
	else
	{
		return false;
	}
}

void ot::StringToVariableConverter::removeThousandsSeparators(std::string& _value, const char _thousandsSeparator)
{
	_value.erase(std::remove(_value.begin(), _value.end(), _thousandsSeparator), _value.end());
}

void ot::StringToVariableConverter::removeWhitespaces(std::string& _value)
{
	_value.erase(std::remove(_value.begin(), _value.end(), ' '), _value.end());
}

void ot::StringToVariableConverter::removeControlCharacters(std::string& _value)
{
	_value.erase(std::remove(_value.begin(), _value.end(), '\n'), _value.end());
	_value.erase(std::remove(_value.begin(), _value.end(), '\r'), _value.end());
	_value.erase(std::remove(_value.begin(), _value.end(), '\t'), _value.end());
}

void ot::StringToVariableConverter::ensurePointAsDecimalSeparator(std::string& _value)
{
	std::replace(_value.begin(), _value.end(), ',', '.');
}

ot::Variable ot::StringToVariableConverter::convertIfFitsNumericalValue(const std::string& _value)
{
	if (ot::StringToNumericCheck::fitsInInt32(_value))
	{
		const int32_t transformedValue = std::stoi(_value);
		return ot::Variable(transformedValue);
	}
	else if (ot::StringToNumericCheck::fitsInInt64(_value))
	{
		const int64_t transformedValue = std::stoll(_value);
		return ot::Variable(transformedValue);
	}
	else if (ot::StringToNumericCheck::fitsInFloat(_value))
	{
		const float transformedValue = std::stof(_value);
		return ot::Variable(transformedValue);
	}
	else if (ot::StringToNumericCheck::fitsInDouble(_value))
	{
		const double transformedValue = std::stod(_value);
		return ot::Variable(transformedValue);
	}
	else
	{
		return ot::Variable(_value);
	}
}