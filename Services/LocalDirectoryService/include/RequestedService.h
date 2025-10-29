// @otlicense

//! @file RequestedService.h
//! @author Alexander Kuester (alexk95)
//! @date September 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// LDS header
#include "SupportedService.h"
#include "ServiceStartupData.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/ServiceInitData.h"

class RequestedService {
	OT_DECL_DEFCOPY(RequestedService)
	OT_DECL_DEFMOVE(RequestedService)
	OT_DECL_NODEFAULT(RequestedService)
public:
	RequestedService(ot::ServiceInitData&& _initData, ServiceStartupData&& _startupData);
	RequestedService(const ot::ServiceInitData& _initData, const SupportedService& _supportedServiceInfo);
	~RequestedService() = default;

	void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;

	ot::ServiceInitData& getInitData() { return m_initData; };
	const ot::ServiceInitData& getInitData() const { return m_initData; };

	ServiceStartupData& getStartupData() { return m_startupData; };
	const ServiceStartupData& getStartupData() const { return m_startupData; };

	unsigned int incrStartCounter() { return m_startupData.incrStartCounter(); };
	unsigned int getMaxCrashRestarts() const { return m_startupData.getMaxCrashRestarts(); };

	unsigned int incrIniAttempt() { return m_startupData.incrIniAttempt(); };
	unsigned int getMaxStartupRestarts() const { return m_startupData.getMaxStartupRestarts(); };

private:
	ot::ServiceInitData m_initData;
	ServiceStartupData  m_startupData;

};