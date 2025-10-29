// @otlicense

#include "DataBuffer.h"
#include "OTCore/LogDispatcher.h"
#include "PythonObjectBuilder.h"

DataBuffer& DataBuffer::instance(void) {
	static DataBuffer g_instance;
	return g_instance;
}

void DataBuffer::addNewPortData(const std::string& _portName, const std::string& _serialisedData, const std::string& _serialisedMetaData)
{
	assert(m_portData.find(_portName) == m_portData.end());
	m_portData.insert(std::pair<std::string, PortData>(_portName, PortData(_portName, _serialisedData,_serialisedMetaData, false)));
}

void DataBuffer::overridePortData(const std::string& _portName, const std::string& _serialisedData)
{
	if (m_portData.find(_portName) == m_portData.end())
	{
		const std::string metadata = "{}";
		m_portData.insert(std::pair<std::string, PortData>(_portName, PortData(_portName, _serialisedData,metadata, true)));
	}
	else
	{
		m_portData.find(_portName)->second.overrideData(_serialisedData);
	}
}
void DataBuffer::overridePortMetaData(const std::string& _portName, const std::string& _serialisedMetaData)
{
	if (m_portData.find(_portName) == m_portData.end())
	{
		const std::string data = "{}";
		m_portData.insert(std::pair<std::string, PortData>(_portName, PortData(_portName, data, _serialisedMetaData, true)));
	}
	else
	{
		m_portData.find(_portName)->second.overrideMetaData(_serialisedMetaData);
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
		PortData& portData = portEntry->second;
		
		const ot::JsonValue& data = portData.getData();
		CPythonObjectNew pObject = builder.setString(ot::json::toJson(data));
		Py_INCREF(pObject);
		return pObject;
	}
	else {
		throw std::exception(("Python script tried to get data from a unknown port: " + _portName).c_str());
		return nullptr;
	}
}

PyObject* DataBuffer::getPortMetaData(const std::string& _portName)
{
	auto portEntry = m_portData.find(_portName);
	PythonObjectBuilder builder;
	if (portEntry != m_portData.end()) {
		PortData& portData = portEntry->second;
		
		const ot::JsonValue& data = portData.getMetadata();
		CPythonObjectNew pObject = builder.setString(ot::json::toJson(data));
		Py_INCREF(pObject);
		return pObject;
	}
	else {
		throw std::exception(("Python script tried to get metadata from a unknown port: " + _portName).c_str());
		return nullptr;
	}
}

std::list<PortData*> DataBuffer::getModifiedPortData()
{
	std::list<PortData*> modifiedPorts;
	for (auto& item : m_portData) {
		PortData& portData = item.second;
		if (portData.getModified()) 
		{
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


