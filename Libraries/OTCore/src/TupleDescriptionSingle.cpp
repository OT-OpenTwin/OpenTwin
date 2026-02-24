#include "OTCore/Tuple/TupleDescriptionSingle.h"

TupleInstance TupleDescriptionSingle::createInstance(std::string _unit, std::string _dataType)
{
	TupleInstance single("");
	single.setTupleUnits({ _unit });
	single.setTupleElementDataTypes({ _dataType });
	return single;
}
