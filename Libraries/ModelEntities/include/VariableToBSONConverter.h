// @otlicense

#pragma once
#include <bsoncxx/builder/basic/document.hpp>
#include "OTCore/Variable.h"
#include <bsoncxx/builder/basic/array.hpp>

struct __declspec(dllexport) VariableToBSONConverter
{
	void operator()(bsoncxx::builder::basic::document& doc, const ot::Variable& value, const std::string& fieldName);
	void operator()(bsoncxx::builder::basic::array& array, const ot::Variable& value);
};
