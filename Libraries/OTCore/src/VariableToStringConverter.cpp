#include "OTCore/VariableToStringConverter.h"
#include <cassert>

std::string ot::VariableToStringConverter::operator()(const ot::Variable& value)
{
	if (value.isConstCharPtr())
	{
		return value.getConstCharPtr();
	}
	else if (value.isBool())
	{
		std::string retVal;
		value.getBool() ? retVal = "TRUE" : retVal = "FALSE";
		return retVal;
	}
	else if (value.isDouble())
	{
		return std::to_string(value.getDouble());
	}
	else if (value.isFloat())
	{
		return std::to_string(value.getFloat());
	}
	else if (value.isInt32())
	{
		return std::to_string(value.getInt32());
	}
	else
	{
		assert(value.isInt64());
		return std::to_string(value.getInt64());
	}
}
