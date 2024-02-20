#pragma once
#include <string>
#include <list>
#include <optional>
#include <map>

#include "OTCore/Variable.h"
#include "OTCore/ReturnValues.h"
#include "PortData.h"
#include "CPythonObject.h"

class PortDataBuffer
{
public:
	static PortDataBuffer& INSTANCE()
	{
		static PortDataBuffer instance;
		return instance;
	}

	void addNewPortData(const std::string& portName,const ot::GenericDataStructList& data);
	void OverridePortData(const std::string& portName,const ot::GenericDataStructList& data);
	void clearPortData();
	PyObject* getPortData(const std::string& portName);
	void AddModifiedPortData(ot::ReturnValues& returnValues);

private:
	std::map<std::string, PortData> _portData;
	PortDataBuffer() {};
};
