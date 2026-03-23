// @otlicense

// OpenTwin header
#include "OTCore/Tuple/TupleDescriptionSingle.h"

ot::TupleInstance ot::TupleDescriptionSingle::createInstance(std::string _unit, std::string _dataType)
{
	TupleInstance single("");
	single.setTupleUnits({ _unit });
	single.setTupleElementDataTypes({ _dataType });
	return single;
}
