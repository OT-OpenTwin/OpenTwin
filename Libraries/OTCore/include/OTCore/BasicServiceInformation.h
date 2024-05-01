//! @file BasicServiceInformation.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"

// std header
#include <string>

namespace ot {

	class OT_CORE_API_EXPORT BasicServiceInformation : public ot::Serializable {
	public:
		BasicServiceInformation();
		BasicServiceInformation(const std::string& _serviceNameAndType);
		BasicServiceInformation(const std::string& _serviceName, const std::string& _serviceType);
		BasicServiceInformation(const BasicServiceInformation& _other);
		virtual ~BasicServiceInformation();

		BasicServiceInformation& operator = (const BasicServiceInformation& _other);
		bool operator == (const BasicServiceInformation& _other) const;
		bool operator != (const BasicServiceInformation& _other) const;
		bool operator < (const BasicServiceInformation& _other) const;

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		void setServiceName(const std::string& _serviceName) { m_name = _serviceName; };
		const std::string& serviceName(void) const { return m_name; };

		void setServiceType(const std::string& _serviceType) { m_type = _serviceType; };
		const std::string& serviceType(void) const { return m_type; };

	private:
		std::string m_name;
		std::string m_type;

	};

}