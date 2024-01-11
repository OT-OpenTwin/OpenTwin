#include "PortData.h"
#include "OTCore/JSON.h"

sp::PortData::PortData(const uint32_t portNumber)
	:_numberOfEntries(portNumber*portNumber)
{
	_portData.reserve(_numberOfEntries);
}

sp::PortData::PortData(PortData&& other)
	:_numberOfEntries(other._numberOfEntries),_frequency(other._frequency), _portData(std::move(other._portData)), _buffer(other._buffer)
{
	other._frequency = nullptr;
	other._buffer = nullptr;
}

sp::PortData& sp::PortData::operator=(PortData&& other)
{
	_numberOfEntries = other._numberOfEntries;
	
	_frequency = other._frequency;
	other._frequency = nullptr;

	_portData = (std::move(other._portData));

	_buffer = (other._buffer);
	_buffer = nullptr;

	return *this;
	// TODO: insert return statement here
}

sp::PortData::~PortData()
{
	if (_frequency != nullptr)
	{
		delete _frequency;
		_frequency = nullptr;
	}
	if (_buffer != nullptr)
	{
		delete _buffer;
		_buffer = nullptr;
	}
}


bool sp::PortData::AddValue(const std::string& value)
{
	if (isFilled())
	{
		return false;
	}
	else
	{
		if (_frequency == nullptr)
		{
			_frequency = new PortDataSingleEntry(StringToPortDataSingleEntry(value));
		}
		else
		{
			if (_buffer == nullptr)
			{
				_buffer = new PortDataSingleEntry(StringToPortDataSingleEntry(value));
			}
			else
			{
				const PortDataEntry entry(std::move(*_buffer), StringToPortDataSingleEntry(value));
				_portData.push_back(entry);
				_buffer = nullptr;
			}
		}
		return true;
	}
}

sp::PortDataSingleEntry sp::PortData::StringToPortDataSingleEntry(const std::string& value)
{
	const std::string valueString = "{\"value\":" + value + "}";
	ot::JsonDocument doc;
	doc.fromJson(valueString);
	if (doc["value"].IsFloat())
	{
		return sp::PortDataSingleEntry(doc["value"].GetFloat());
	}
	else if (doc["value"].IsDouble())
	{
		return sp::PortDataSingleEntry(doc["value"].GetDouble());
	}
	else if (doc["value"].IsInt())
	{
		return sp::PortDataSingleEntry(doc["value"].GetInt());
	}
	else if (doc["value"].IsInt64())
	{
		return sp::PortDataSingleEntry(doc["value"].GetInt64());
	}
	else
	{
		throw std::exception(std::string("Failed to convert \"" + value + "\" into a floating point or integer value.").c_str());
	}
	return PortDataSingleEntry();
}
