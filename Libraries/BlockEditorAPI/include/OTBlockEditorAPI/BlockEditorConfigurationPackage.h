//! @file BlockEditorConfigurationPackage.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditorAPI/BlockEditorAPIAPIExport.h"
#include "OpenTwinCore/Serializable.h"

// std header
#include <string>
#include <list>

#pragma warning (disable:4251)

namespace ot {

	class BlockCategoryConfiguration;

	class BLOCKEDITORAPI_API_EXPORT BlockEditorConfigurationPackage : public ot::Serializable {
	public:
		BlockEditorConfigurationPackage() {};
		BlockEditorConfigurationPackage(const std::string& _editorName, const std::string& _editorTitle);
		~BlockEditorConfigurationPackage();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		void setEditorName(const std::string& _name) { m_editorName = _name; };
		const std::string& editorName(void) const { return m_editorName; };

		void setEditorTitle(const std::string& _title) { m_editorTitle = _title; };
		const std::string& editorTitle(void) const { return m_editorTitle; };

		void setTopLevelBlockCategories(const std::list<ot::BlockCategoryConfiguration*>& _categories);
		void addTopLevelBlockCategory(ot::BlockCategoryConfiguration* _category);
		void addTopLevelBlockCategories(const std::list<ot::BlockCategoryConfiguration*>& _categories);
		const std::list<ot::BlockCategoryConfiguration*>& topLevelBlockCategories(void) const { return m_topLevelBlockCategories; };

	private:
		void memClear(void);

		std::string m_editorName;
		std::string m_editorTitle;
		std::list<ot::BlockCategoryConfiguration*> m_topLevelBlockCategories;
		BlockEditorConfigurationPackage(const BlockEditorConfigurationPackage&) = delete;
		BlockEditorConfigurationPackage& operator = (const BlockEditorConfigurationPackage&) = delete;
	};

}