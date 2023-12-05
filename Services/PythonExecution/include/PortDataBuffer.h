#pragma once
#include <string>
#include <list>
#include <optional>
#include <map>

#include "OpenTwinCore/Variable.h"
#include "OpenTwinCore/ReturnValues.h"
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

	void addNewPortData(const std::string& portName,const std::list<ot::Variable>& data);
	void addOrOverridePortData(const std::string& portName,const std::list<ot::Variable>& data);
	void clearPortData();
	PyObject* getPortData(const std::string& portName);
	void AddModifiedPortData(ot::ReturnValues& returnValues);

private:
	std::map<std::string, PortData> _portData;
	PortDataBuffer() {};
};
