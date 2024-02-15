#pragma once
#include "OTCore/GenericDataStruct.h"
#include <list>
#include <memory>

class PortData
{
public:
	PortData(const std::string& portName, const ot::GenericDataStructList& values, bool modified = false);
	PortData(PortData&& other) noexcept;
	PortData(const PortData& other) = delete;
	PortData& operator=(PortData&& other) noexcept;
	PortData& operator=(const PortData& other) = delete;
	~PortData();
	void overrideValues(const ot::GenericDataStructList& values);
	const ot::GenericDataStructList& getValues() const;
	const bool getModified() const;
	ot::GenericDataStructList HandDataOver();
private:
	std::string _portName;
	bool _modified;
	ot::GenericDataStructList _values;

	void FreeMemory();
};
