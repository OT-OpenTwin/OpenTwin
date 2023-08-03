//! @file BlockCategoryConfiguration.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditorAPI/BlockEditorAPIAPIExport.h"
#include "OTBlockEditorAPI/BlockConfigurationObject.h"

// std header
#include <string>
#include <list>

#pragma warning (disable:4251)

namespace ot {

	class BlockConfiguration;

	class BLOCKEDITORAPI_API_EXPORT BlockCategoryConfiguration : public ot::BlockConfigurationObject {
	public:
		BlockCategoryConfiguration(const std::string& _name = std::string(), const std::string& _title = std::string());
		virtual ~BlockCategoryConfiguration();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		// ########################################################################################################################################################

		// Setter/Getter

		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string& title(void) const { return m_title; };

		void setIconSubPath(const std::string& _path) { m_iconSubPath = _path; };
		const std::string& iconSubPath(void) const { return m_iconSubPath; };

		BlockCategoryConfiguration* addChild(const std::string& _name);
		void addChild(BlockCategoryConfiguration* _category);
		const std::list<BlockCategoryConfiguration*>& childs(void) const { return m_childs; };

		void addItem(BlockConfiguration* _item);
		const std::list<BlockConfiguration*>& items(void) const { return m_items; };

	private:
		inline void setParentCategory(BlockCategoryConfiguration* _category) { m_parentCategory = _category; };
		inline BlockCategoryConfiguration* parentCategory(void) const { return m_parentCategory; };

		std::string m_title;
		BlockCategoryConfiguration* m_parentCategory;
		std::string m_iconSubPath;
		std::list<BlockCategoryConfiguration*> m_childs;
		std::list<BlockConfiguration*> m_items;

		BlockCategoryConfiguration(BlockCategoryConfiguration&) = delete;
		BlockCategoryConfiguration& operator = (BlockCategoryConfiguration&) = delete;
	};

}