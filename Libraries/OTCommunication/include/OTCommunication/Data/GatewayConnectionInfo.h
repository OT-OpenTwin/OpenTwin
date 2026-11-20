// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
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

		void setGatewayUrl(const std::string& _gatewayUrl) { m_gatewayUrl = _gatewayUrl; };
		const std::string& getGatewayUrl() const { return m_gatewayUrl; };

		void setGatewayServiceUrl(const std::string& _gatewayServiceUrl) { m_gatewayServiceUrl = _gatewayServiceUrl; };
		const std::string& getGatewayServiceUrl() const { return m_gatewayServiceUrl; };

	private:
		std::string m_gatewayUrl;
		std::string m_gatewayServiceUrl;

	};

}