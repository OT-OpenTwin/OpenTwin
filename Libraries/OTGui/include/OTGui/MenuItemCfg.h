//! @file MenuItemCfg.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/MenuEntryCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT MenuItemCfg : public MenuEntryCfg {
	public:
		MenuItemCfg();
		MenuItemCfg(const std::string& _text, const std::string& _iconPath = std::string(), const std::string& _shortcut = std::string());
		MenuItemCfg(const MenuItemCfg& _other);
		MenuItemCfg(const ot::ConstJsonObject& _object);
		virtual ~MenuItemCfg();

		MenuItemCfg& operator = (const MenuItemCfg&) = delete;

		virtual MenuEntryCfg* createCopy(void) const override;
		virtual EntryType getMenuEntryType(void) const override { return MenuEntryCfg::Item; };

		//! \brief Add the object contents to the provided JSON object.
		//! \param _object Json object reference to write the data to.
		//! \param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! \brief Set the object contents from the provided JSON object.
		//! \param _object The JSON object containing the information.
		//! \throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

	private:
		std::string m_text;
		std::string m_iconPath;
		std::string m_shortcut;
	};

}