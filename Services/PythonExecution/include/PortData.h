#pragma once
#include "OTCore/Variable.h"
#include <list>

class PortData
{
public:
	PortData(const std::string& portName, const std::list<ot::Variable> & values, bool modified = false);
	void overrideValues(const std::list<ot::Variable>& values);
	const std::list<ot::Variable>& getValues() const;
	const bool getModified() const;

private:
	std::string _portName;
	bool _modified;
	std::list<ot::Variable> _values;
};
