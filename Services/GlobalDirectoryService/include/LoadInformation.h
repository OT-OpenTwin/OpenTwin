//! @file LoadInformation.h
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"

//! @brief The LoadInformation class provides information about the current system load.
class LoadInformation {
public:
	typedef unsigned long long memory_t;
	typedef double load_t;
	static load_t maxLoadValue(void) { return DBL_MAX; }

	LoadInformation();
	LoadInformation(const LoadInformation& _other);
	LoadInformation(LoadInformation&& _other) noexcept;
	virtual ~LoadInformation();

	LoadInformation& operator = (const LoadInformation& _other);
	LoadInformation& operator = (LoadInformation&& _other) noexcept;

	load_t load(void) const;
	bool updateSystemUsageValues(ot::JsonDocument& _jsonDocument);

private:
	load_t calculateLoad(memory_t _total, memory_t _avail);

	load_t   m_currentPhysicalMemoryLoad;
	load_t   m_currentVirtualMemoryLoad;

	memory_t m_totalPhysicalMemory;
	memory_t m_availablePhysicalMemory;
	memory_t m_totalVirtualMemory;
	memory_t m_availableVirtualMemory;
};