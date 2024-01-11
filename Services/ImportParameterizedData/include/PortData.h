#pragma once
#include "OTCore/Variable.h"

#include <stdint.h>
#include <vector>
#include <tuple>

namespace sp
{
	typedef std::variant<int32_t, int64_t, float, double> PortDataSingleEntry;
	typedef std::tuple<PortDataSingleEntry, PortDataSingleEntry> PortDataEntry;

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
		const PortDataSingleEntry& getFrequency() const { return *_frequency; }
		const bool isFilled() { return static_cast<uint32_t>(_portData.size()) == _numberOfEntries;}
	
	private:	
		uint32_t _numberOfEntries;
		PortDataSingleEntry* _frequency = nullptr;
		std::vector<PortDataEntry> _portData;
		PortDataSingleEntry* _buffer = nullptr;

		PortDataSingleEntry StringToPortDataSingleEntry(const std::string& value);
	};
}
