// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTCommunication/CommunicationAPIExport.h"

namespace ot
{

	class OT_COMMUNICATION_API_EXPORT GatewayInitData : public Serializable
	{
		OT_DECL_DEFCOPY(GatewayInitData)
		OT_DECL_DEFMOVE(GatewayInitData)
	public:
		GatewayInitData() = default;
		virtual ~GatewayInitData() = default;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;
		
		void setProjectId(const std::string& _projectId) { m_projectId = _projectId; };
		const std::string& getProjectId() const { return m_projectId; };

	private:
		std::string m_projectId;

	};

}