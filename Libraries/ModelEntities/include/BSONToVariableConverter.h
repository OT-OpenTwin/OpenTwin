// @otlicense

#pragma once
#include "OTCore/Variable.h"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>

class __declspec(dllexport) BSONToVariableConverter
{
public:
	ot::Variable operator()(const bsoncxx::v_noabi::document::element& element);
	ot::Variable operator()(const bsoncxx::v_noabi::array::element& element);
};
