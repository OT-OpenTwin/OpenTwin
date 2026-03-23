// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/TypeNames.h"
#include "OTCore/ComplexNumbers/ComplexNumberFormat.h"
#include "OTCore/Tuple/TupleDescription.h"

namespace ot {

	class OT_CORE_API_EXPORT TupleDescriptionSingle : public TupleDescription
	{
	public:
		static TupleInstance createInstance(std::string _unit, std::string _dataType);
	};

}
