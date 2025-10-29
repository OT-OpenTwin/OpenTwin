// @otlicense

#include "VariableToBSONConverter.h"
#include<cassert>

void VariableToBSONConverter::operator()(bsoncxx::builder::basic::document& doc, const ot::Variable& value, const std::string& fieldName)
{
	if (value.isBool())
	{
		doc.append(bsoncxx::builder::basic::kvp(fieldName, value.getBool()));
	}
	else if (value.isConstCharPtr())
	{
		doc.append(bsoncxx::builder::basic::kvp(fieldName, value.getConstCharPtr()));
	}
	else if (value.isDouble())
	{
		doc.append(bsoncxx::builder::basic::kvp(fieldName, value.getDouble()));
	}
	else if (value.isFloat())
	{
		doc.append(bsoncxx::builder::basic::kvp(fieldName, value.getFloat()));
	}
	else if (value.isInt32())
	{
		doc.append(bsoncxx::builder::basic::kvp(fieldName, value.getInt32()));
	}
	else if (value.isInt64())
	{
		doc.append(bsoncxx::builder::basic::kvp(fieldName, value.getInt64()));
	}
	else
	{
		assert(0);
	}
}

void VariableToBSONConverter::operator()(bsoncxx::builder::basic::array& array, const ot::Variable& value)
{
	if (value.isBool())
	{
		array.append(value.getBool());
	}
	else if (value.isConstCharPtr())
	{
		array.append(value.getConstCharPtr());
	}
	else if (value.isDouble())
	{
		array.append(value.getDouble());
	}
	else if (value.isFloat())
	{
		array.append(value.getFloat());
	}
	else if (value.isInt32())
	{
		array.append(value.getInt32());
	}
	else if (value.isInt64())
	{
		array.append(value.getInt64());
	}
	else
	{
		assert(0);
	}
}
