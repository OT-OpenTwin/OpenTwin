//! @file ProjectTemplateInformation.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTCore/CoreAPIExport.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_CORE_API_EXPORT ProjectTemplateInformation : public Serializable {
	public:
		ProjectTemplateInformation();
		ProjectTemplateInformation(const ProjectTemplateInformation& _other);
		virtual ~ProjectTemplateInformation();

		ProjectTemplateInformation& operator = (const ProjectTemplateInformation& _other);

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName(void) const { return m_name; };

		void setDescription(const std::string& _description) { m_description = _description; };
		const std::string& getDescription(void) const { return m_description; };

		void setIconSubPath(const std::string& _path) { m_iconSubPath = _path; };
		const std::string& getIconSubPath(void) const { return m_iconSubPath; };

		void setIsDefault(bool _isDefault) { m_isDefault = _isDefault; };
		bool getIsDefault(void) const { return m_isDefault; };

	private:
		std::string m_name;
		std::string m_description;
		std::string m_iconSubPath;
		bool m_isDefault;
	};

}