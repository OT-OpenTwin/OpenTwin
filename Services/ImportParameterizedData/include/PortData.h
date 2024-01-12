#pragma once
#include "OTCore/Variable.h"

#include <stdint.h>
#include <vector>
#include "OTCore/Variable.h"

namespace ts
{
	typedef std::tuple<ot::Variable, ot::Variable> PortDataEntry;

	class PortData
	{
	public:
		PortData(const uint32_t portNumber);
		const PortData(const PortData& other) = delete;
		const PortData(PortData&& other);
		PortData& operator=(const PortData& other) = delete;
		PortData& operator=(PortData&& other);


		~PortData();
		bool AddValue(const std::string& value);
		const std::vector<PortDataEntry>& getPortDataEntries() const { return _portData; }
		const ot::Variable& getFrequency() const { return *_frequency; }
		const bool isFilled() { return static_cast<uint32_t>(_portData.size()) == _numberOfEntries;}
	
	private:	
		uint32_t _numberOfEntries;
		ot::Variable* _frequency = nullptr;
		std::vector<PortDataEntry> _portData;
		ot::Variable* _buffer = nullptr;

		ot::Variable StringToPortDataSingleEntry(const std::string& value);
	};
}
