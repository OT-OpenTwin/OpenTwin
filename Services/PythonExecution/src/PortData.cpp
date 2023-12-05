#include "PortData.h"

PortData::PortData(const std::string& portName, const std::list<ot::Variable>& values, bool modified )
	:_portName(portName), _modified(modified)
{
	_values = (values);
}

void PortData::overrideValues(const std::list<ot::Variable>& values)
{
	_values = values;
	_modified = true;
}

const std::list<ot::Variable>& PortData::getValues() const
{
	return _values;
}

const bool PortData::getModified() const
{
	return _modified;
}
