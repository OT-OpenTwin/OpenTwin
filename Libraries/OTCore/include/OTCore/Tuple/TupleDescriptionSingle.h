// @otlicense
#pragma once
#include "OTCore/Tuple/TupleDescription.h"
#include "OTCore/ComplexNumbers/ComplexNumberFormat.h"
#include "OTCore/TypeNames.h"
class OT_CORE_API_EXPORT TupleDescriptionSingle : public TupleDescription
{
public:
	static TupleInstance createInstance(std::string _unit, std::string _dataType);
};
