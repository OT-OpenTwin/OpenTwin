#include "PortDataBuffer.h"
#include "OTCore/Logger.h"
#include "PythonObjectBuilder.h"

PortDataBuffer& PortDataBuffer::instance(void) {
	static PortDataBuffer g_instance;
	return g_instance;
}

void PortDataBuffer::addNewPortData(const std::string& _portName, const ot::GenericDataStructList& _data)
{
	assert(m_portData.find(_portName) == m_portData.end());
	m_portData.insert(std::pair<std::string, PortData>(_portName, PortData(_portName, _data, false)));
}

void PortDataBuffer::overridePortData(const std::string& _portName, const ot::GenericDataStructList& _data)
{
	if (m_portData.find(_portName) == m_portData.end())
	{
		m_portData.insert(std::pair<std::string, PortData>(_portName, PortData(_portName, _data, true)));
	}
	else
	{
		m_portData.find(_portName)->second.overrideValues(_data);
	}
}
void PortDataBuffer::clearPortData()
{
	m_portData.clear();
}

PyObject* PortDataBuffer::getPortData(const std::string& _portName)
{
	auto portEntry = m_portData.find(_portName);
	PythonObjectBuilder builder;
	if (portEntry != m_portData.end()) {
		ot::GenericDataStructList values = portEntry->second.getValues();
		CPythonObjectNew pObject = builder.setGenericDataStructList(values);
		Py_INCREF(pObject);
		return pObject;
	}
	else {
		throw std::exception("Python script tried to get data from a unknown port .");
		return nullptr;
	}
}

void PortDataBuffer::addModifiedPortData(ot::ReturnValues& _returnValues)
{
	for (auto& item : m_portData) {
		PortData& portData = item.second;
		if (portData.getModified()) {
			_returnValues.addData(item.first, std::move(portData.HandDataOver()));
		}
	}
}
