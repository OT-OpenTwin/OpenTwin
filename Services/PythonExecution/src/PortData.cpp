#include "PortData.h"

PortData::PortData(const std::string& portName, const ot::GenericDataStructList& values, bool modified )
	:_portName(portName), _modified(modified)
{
	_values = (values);
}

PortData::PortData(PortData&& other) noexcept
	:_portName(other._portName), _values(std::move(other._values))
{
}

PortData& PortData::operator=(PortData&& other) noexcept
{
	_portName = (other._portName);
	_values = (std::move(other._values));
	return *this;
}

PortData::~PortData()
{
	FreeMemory();
}

void PortData::overrideValues(const ot::GenericDataStructList& values)
{
	FreeMemory();
	_values = values;
	_modified = true;
}

const ot::GenericDataStructList& PortData::getValues() const
{
	return _values;
}

const bool PortData::getModified() const
{
	return _modified;
}

ot::GenericDataStructList PortData::HandDataOver()
{
	ot::GenericDataStructList values(_values);
	_values.clear();
	return values;
}

void PortData::FreeMemory()
{
	for (ot::GenericDataStruct* value : _values)
	{
		delete value;
		value = nullptr;
	}
}
