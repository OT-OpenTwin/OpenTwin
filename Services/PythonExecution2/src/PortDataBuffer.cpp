#include "PortDataBuffer.h"
#include "OTCore/Logger.h"
#include "PythonObjectBuilder.h"

PortDataBuffer& PortDataBuffer::instance(void) {
	static PortDataBuffer g_instance;
	return g_instance;
}

void PortDataBuffer::addNewPortData(const std::string& portName, const ot::GenericDataStructList& data)
{
	assert(_portData.find(portName) == _portData.end());
	_portData.insert(std::pair<std::string, PortData>(portName, PortData(portName, data, false)));
}

void PortDataBuffer::OverridePortData(const std::string& portName, const ot::GenericDataStructList& data)
{
	if (_portData.find(portName) == _portData.end())
	{
		_portData.insert(std::pair<std::string, PortData>(portName, PortData(portName, data, true)));
	}
	else
	{
		_portData.find(portName)->second.overrideValues(data);
	}
}
void PortDataBuffer::clearPortData()
{
	_portData.clear();
}

PyObject* PortDataBuffer::getPortData(const std::string& portName)
{
	auto portEntry = _portData.find(portName);
	PythonObjectBuilder builder;
	if (portEntry != _portData.end())
	{
		ot::GenericDataStructList values = portEntry->second.getValues();
		CPythonObjectNew pObject = builder.setGenericDataStructList(values);
		Py_INCREF(pObject);
		return pObject;
	}
	else
	{
		throw std::exception("Python script tried to get data from a unknown port .");
		return nullptr;
	}
}

void PortDataBuffer::AddModifiedPortData(ot::ReturnValues& returnValues)
{
	for (auto& item : _portData)
	{
		PortData& portData = item.second;
		if (portData.getModified())
		{
			returnValues.addData(item.first, std::move(portData.HandDataOver()));
		}
	}
}
