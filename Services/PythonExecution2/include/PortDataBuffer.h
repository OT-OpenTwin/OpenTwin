#pragma once
#include <string>
#include <list>
#include <optional>
#include <map>

#include "OTCore/Variable.h"
#include "OTCore/ReturnValues.h"
#include "PortData.h"
#include "CPythonObject.h"

class PortDataBuffer {
public:
	static PortDataBuffer& instance(void);

	void addNewPortData(const std::string& _portName, const ot::GenericDataStructList& _data);
	void overridePortData(const std::string& _portName, const ot::GenericDataStructList& _data);
	void clearPortData();
	PyObject* getPortData(const std::string& _portName);
	void addModifiedPortData(ot::ReturnValues& _returnValues);

private:
	std::map<std::string, PortData> m_portData;

	PortDataBuffer() {};
};
