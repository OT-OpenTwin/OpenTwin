#pragma once
#include <string>
#include <list>
#include <optional>
#include <map>

#include "CPythonObjectNew.h"
#include "OpenTwinCore/Variable.h"

class PortDataBuffer
{
public:
	static PortDataBuffer& INSTANCE()
	{
		static PortDataBuffer instance;
		return instance;
	}

	void addPortData(const std::string& portName,CPythonObjectNew&& data);
	void clearPortData();
	PyObject* getPortData(const std::string& portName);

private:
	std::map<std::string, CPythonObjectNew> _portData;

	PortDataBuffer() {};
};
