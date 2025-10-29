// @otlicense

#include "OTCore/VariableToBSONStringConverter.h"
#include <cassert>

std::string ot::VariableToBSONStringConverter::operator()(const ot::Variable& variable)
{
	std::string bsonString;
	if (variable.isConstCharPtr())
	{
		bsonString = "\"" + std::string(variable.getConstCharPtr()) + "\"";
	}
	else
	{
		if (variable.isDouble())
		{
			bsonString = std::to_string(variable.getDouble());
		}
		else if (variable.isFloat())
		{
			bsonString = std::to_string(variable.getFloat());
		}
		else if (variable.isInt64())
		{
			bsonString = std::to_string(variable.getInt64());
		}
		else if (variable.isInt32())
		{
			bsonString = std::to_string(variable.getInt32());
		}
		else if (variable.isBool())
		{
			if (variable.getBool())
			{
				bsonString ="true";
			}
			else
			{
				bsonString = "false";
			}
		}
		else
		{
			assert(0);
		}
	}
    return bsonString;
}
