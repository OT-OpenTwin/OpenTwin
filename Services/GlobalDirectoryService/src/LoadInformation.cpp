//! @file LoadInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// GDS header
#include "LoadInformation.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/String.h"
#include "OTCommunication/ActionTypes.h"

LoadInformation::LoadInformation() 
	: m_currentPhysicalMemoryLoad(100.), m_currentVirtualMemoryLoad(100.),
	m_totalPhysicalMemory(0), m_totalVirtualMemory(0), m_availablePhysicalMemory(0), m_availableVirtualMemory(0)
{

}

LoadInformation::LoadInformation(const LoadInformation& _other) {
	m_currentPhysicalMemoryLoad = _other.m_currentPhysicalMemoryLoad;
	m_currentVirtualMemoryLoad = _other.m_currentVirtualMemoryLoad;

	m_totalPhysicalMemory = _other.m_totalPhysicalMemory;
	m_totalVirtualMemory = _other.m_totalVirtualMemory;

	m_availablePhysicalMemory = _other.m_availablePhysicalMemory;
	m_availableVirtualMemory = _other.m_availableVirtualMemory;
}

LoadInformation::~LoadInformation() {

}

LoadInformation& LoadInformation::operator = (const LoadInformation& _other) {
	m_currentPhysicalMemoryLoad = _other.m_currentPhysicalMemoryLoad;
	m_currentVirtualMemoryLoad = _other.m_currentVirtualMemoryLoad;

	m_totalPhysicalMemory = _other.m_totalPhysicalMemory;
	m_totalVirtualMemory = _other.m_totalVirtualMemory;

	m_availablePhysicalMemory = _other.m_availablePhysicalMemory;
	m_availableVirtualMemory = _other.m_availableVirtualMemory;

	return *this;
}

LoadInformation::load_t LoadInformation::load(void) const {
	// Determine max load value
	load_t l = m_currentPhysicalMemoryLoad;
	if (m_currentVirtualMemoryLoad > l) l = m_currentVirtualMemoryLoad;
	return l;
}

bool LoadInformation::updateSystemUsageValues(ot::JsonDocument& _jsonDocument) {
	if (!_jsonDocument.HasMember(OT_ACTION_PARAM_SYSTEM_AvailablePhysicalMemory)) { OT_LOG_W("JSON member missing: " OT_ACTION_PARAM_SYSTEM_AvailablePhysicalMemory); return false; }
	if (!_jsonDocument.HasMember(OT_ACTION_PARAM_SYSTEM_AvailableVirtualMemory)) { OT_LOG_W("JSON member missing: " OT_ACTION_PARAM_SYSTEM_AvailableVirtualMemory); return false; }
	if (!_jsonDocument.HasMember(OT_ACTION_PARAM_SYSTEM_TotalPhysicalMemory)) { OT_LOG_W("JSON member missing: " OT_ACTION_PARAM_SYSTEM_TotalPhysicalMemory); return false; }
	if (!_jsonDocument.HasMember(OT_ACTION_PARAM_SYSTEM_TotalVirtualMemory)) { OT_LOG_W("JSON member missing: " OT_ACTION_PARAM_SYSTEM_TotalVirtualMemory); return false; }

	if (!_jsonDocument[OT_ACTION_PARAM_SYSTEM_AvailablePhysicalMemory].IsString()) { OT_LOG_W("JSON member is not a string: " OT_ACTION_PARAM_SYSTEM_AvailablePhysicalMemory); return false; }
	if (!_jsonDocument[OT_ACTION_PARAM_SYSTEM_AvailableVirtualMemory].IsString()) { OT_LOG_W("JSON member is not a string: " OT_ACTION_PARAM_SYSTEM_AvailableVirtualMemory); return false; }
	if (!_jsonDocument[OT_ACTION_PARAM_SYSTEM_TotalPhysicalMemory].IsString()) { OT_LOG_W("JSON member is not a string: " OT_ACTION_PARAM_SYSTEM_TotalPhysicalMemory); return false; }
	if (!_jsonDocument[OT_ACTION_PARAM_SYSTEM_TotalVirtualMemory].IsString()) { OT_LOG_W("JSON member is not a string: " OT_ACTION_PARAM_SYSTEM_TotalVirtualMemory); return false; }

	bool convertFailed = false;
	memory_t availablePhysicalMemory = ot::String::toNumber<memory_t>(_jsonDocument[OT_ACTION_PARAM_SYSTEM_AvailablePhysicalMemory].GetString(), convertFailed);
	if (convertFailed) { OT_LOG_W("Convert failed for: " OT_ACTION_PARAM_SYSTEM_AvailablePhysicalMemory); return false; }

	memory_t availableVirtualMemory = ot::String::toNumber<memory_t>(_jsonDocument[OT_ACTION_PARAM_SYSTEM_AvailableVirtualMemory].GetString(), convertFailed);
	if (convertFailed) { OT_LOG_W("Convert failed for: " OT_ACTION_PARAM_SYSTEM_AvailableVirtualMemory); return false; }

	memory_t totalPhysicalMemory = ot::String::toNumber<memory_t>(_jsonDocument[OT_ACTION_PARAM_SYSTEM_TotalPhysicalMemory].GetString(), convertFailed);
	if (convertFailed) { OT_LOG_W("Convert failed for: " OT_ACTION_PARAM_SYSTEM_TotalPhysicalMemory); return false; }

	memory_t totalVirtualMemory = ot::String::toNumber<memory_t>(_jsonDocument[OT_ACTION_PARAM_SYSTEM_TotalVirtualMemory].GetString(), convertFailed);
	if (convertFailed) { OT_LOG_W("Convert failed for: " OT_ACTION_PARAM_SYSTEM_TotalVirtualMemory); return false; }

	m_totalPhysicalMemory = totalPhysicalMemory;
	m_availablePhysicalMemory = availablePhysicalMemory;
	m_totalVirtualMemory = totalVirtualMemory;
	m_availableVirtualMemory = availableVirtualMemory;

	m_currentPhysicalMemoryLoad = calculateLoad(m_totalPhysicalMemory, m_availablePhysicalMemory);
	m_currentVirtualMemoryLoad = calculateLoad(m_totalVirtualMemory, m_availableVirtualMemory);

	return true;
}

LoadInformation::load_t LoadInformation::calculateLoad(memory_t _total, memory_t _avail) {
	if (_total == 0) return 100.;
	memory_t used = _total - _avail;
	double result = ((double)used / (double)_total) * 100.;
	if (result > 100.) result = 100.;
	return result;
}