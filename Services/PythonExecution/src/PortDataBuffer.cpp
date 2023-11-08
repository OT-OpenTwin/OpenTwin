#include "PortDataBuffer.h"
#include "OpenTwinCore/Logger.h"

void PortDataBuffer::addPortData(const std::string& portName, CPythonObjectNew&& data)
{
	_portData.insert(std::pair<std::string, CPythonObjectNew>(portName,std::move(data)));
}

void PortDataBuffer::clearPortData()
{
	_portData.clear();
}

PyObject* PortDataBuffer::getPortData(const std::string& portName)
{
	auto portEntry = _portData.find(portName);
	if (portEntry != _portData.end())
	{
		Py_XINCREF(portEntry->second);
		return portEntry->second;
	}
	else
	{
		OT_LOG_E("Python script tried to get data from a prot that is not known.");
		return nullptr;
	}
	
}
