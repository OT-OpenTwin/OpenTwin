// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTCommunication/CommunicationAPIExport.h"

namespace ot
{

	class OT_COMMUNICATION_API_EXPORT GatewayConnectionInfo : public Serializable
	{
		OT_DECL_DEFCOPY(GatewayConnectionInfo)
		OT_DECL_DEFMOVE(GatewayConnectionInfo)
	public:
		GatewayConnectionInfo() = default;
		virtual ~GatewayConnectionInfo() = default;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		bool isValid() const;

		void setGatewayServiceUrl(const std::string& _gatewayServiceUrl) { m_gatewayServiceUrl = _gatewayServiceUrl; };
		const std::string& getGatewayServiceUrl() const { return m_gatewayServiceUrl; };

	private:
		std::string m_gatewayServiceUrl;

	};

}