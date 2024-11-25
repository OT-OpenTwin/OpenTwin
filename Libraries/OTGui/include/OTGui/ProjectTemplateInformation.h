//! @file ProjectTemplateInformation.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/StyledTextBuilder.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT ProjectTemplateInformation : public Serializable {
	public:
		ProjectTemplateInformation();
		ProjectTemplateInformation(const ProjectTemplateInformation& _other);
		virtual ~ProjectTemplateInformation();

		ProjectTemplateInformation& operator = (const ProjectTemplateInformation& _other);

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName(void) const { return m_name; };

		void setProjectType(const std::string& _type) { m_projectType = _type; };
		const std::string& getProjectType(void) const { return m_projectType; };

		void setBriefDescription(const std::string& _description) { m_briefDescription = _description; };
		const std::string& getBriefDescription(void) const { return m_briefDescription; };

		void setDescription(const StyledTextBuilder& _description) { m_description = _description; };
		const StyledTextBuilder& getDescription(void) const { return m_description; };

		void setIsDefault(bool _isDefault) { m_isDefault = _isDefault; };
		bool getIsDefault(void) const { return m_isDefault; };

	private:
		std::string m_name;
		std::string m_projectType;
		std::string m_briefDescription;
		StyledTextBuilder m_description;
		bool m_isDefault;
	};

}