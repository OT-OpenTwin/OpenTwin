// @otlicense

#pragma once

// OpenTwin header
#include "OTCommunication/Data/ServiceInitData.h"

namespace ot
{

	class OT_COMMUNICATION_API_EXPORT GatewayInitData : public ServiceInitData
	{
		OT_DECL_DEFCOPY(GatewayInitData)
		OT_DECL_DEFMOVE(GatewayInitData)
	public:
		GatewayInitData();
		GatewayInitData(ServiceInitData&& _serviceInitData);
		GatewayInitData(const ConstJsonObject& _jsonObject);
		virtual ~GatewayInitData() = default;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;
		
	};

}