// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/ServiceBase.h"
#include "OTCommunication/CommunicationAPIExport.h"

namespace ot
{

	class OT_COMMUNICATION_API_EXPORT SyncServiceStartInfo : public Serializable
	{
		OT_DECL_DEFCOPY(SyncServiceStartInfo)
		OT_DECL_DEFMOVE(SyncServiceStartInfo)
	public:
		SyncServiceStartInfo() = default;
		virtual ~SyncServiceStartInfo() = default;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		bool isValid() const;

		void setServiceInfo(const ServiceBase& _serviceInfo) { m_seviceInfo = _serviceInfo; };
		void setServiceInfo(ServiceBase&& _serviceInfo) { m_seviceInfo = std::move(_serviceInfo); };
		ServiceBase& getServiceInfo() { return m_seviceInfo; };
		const ServiceBase& getServiceInfo() const { return m_seviceInfo; };

	private:
		ServiceBase m_seviceInfo;

	};

}