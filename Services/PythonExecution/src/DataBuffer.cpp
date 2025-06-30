#include "DataBuffer.h"
#include "OTCore/Logger.h"
#include "PythonObjectBuilder.h"

DataBuffer& DataBuffer::instance(void) {
	static DataBuffer g_instance;
	return g_instance;
}

void DataBuffer::addNewPortData(const std::string& _portName, const std::string& _serialisedData)
{
	assert(m_portData.find(_portName) == m_portData.end());
	m_portData.insert(std::pair<std::string, PortData>(_portName, PortData(_portName, _serialisedData, false)));
}

void DataBuffer::overridePortData(const std::string& _portName, const std::string& _serialisedData)
{
	if (m_portData.find(_portName) == m_portData.end())
	{
		m_portData.insert(std::pair<std::string, PortData>(_portName, PortData(_portName, _serialisedData, true)));
	}
	else
	{
		m_portData.find(_portName)->second.overrideValues(_serialisedData);
	}
}
void DataBuffer::clearData()
{
	m_portData.clear();
}

PyObject* DataBuffer::getPortData(const std::string& _portName)
{
	auto portEntry = m_portData.find(_portName);
	PythonObjectBuilder builder;
	if (portEntry != m_portData.end()) {
		const ot::JsonDocument&	data = portEntry->second.getValues();
		CPythonObjectNew pObject = builder.setString(data.toJson());
		Py_INCREF(pObject);
		return pObject;
	}
	else {
		throw std::exception("Python script tried to get data from a unknown port .");
		return nullptr;
	}
}

std::list<PortData*> DataBuffer::getModifiedPortData()
{
	std::list<PortData*> modifiedPorts;
	for (auto& item : m_portData) {
		PortData& portData = item.second;
		if (portData.getModified()) {
			modifiedPorts.push_back(&portData);
		}
	}
	return modifiedPorts;
}

void DataBuffer::addReturnData(const std::string& _scriptName, const std::string& _returnData)
{
	ot::JsonDocument doc;
	doc.fromJson(_returnData);
	m_returnDataByScriptName[_scriptName] =std::move(doc);
}


