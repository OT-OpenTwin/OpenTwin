//! @file MenuCfg.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/MenuButtonCfg.h"
#include "OTGui/MenuClickableEntryCfg.h"

// std header
#include <list>

namespace ot {

	class OT_GUI_API_EXPORT MenuCfg : public MenuClickableEntryCfg {
	public:
		MenuCfg();
		MenuCfg(const std::string& _name, const std::string& _text, const std::string& _iconPath = std::string());
		MenuCfg(const MenuCfg& _other);
		MenuCfg(const ot::ConstJsonObject& _object);
		virtual ~MenuCfg();

		MenuCfg& operator = (const MenuCfg& _other);

		virtual MenuEntryCfg* createCopy(void) const override;
		virtual EntryType getMenuEntryType(void) const override { return MenuEntryCfg::Menu; };

		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! @brief Adds the provided entry to this menu.
		//! @param _entry Entry to add. The menu takes ownership of the entry.
		void add(MenuEntryCfg* _entry);

		//! @brief Creates and adds a child menu.
		//! The menu takes ownership of the created child menu.
		//! @param _text Menu text.
		//! @param _iconPath Menu icon path.
		//! @param _shortcut Menu shortcut.
		MenuCfg* addMenu(const std::string& _name, const std::string& _text, const std::string& _iconPath = std::string());
		
		//! @brief Creates and adds a child item.
		//! The menu takes ownership of the created child item.
		//! @param _text Item text.
		//! @param _iconPath Item icon path.
		//! @param _shortcut Item shortcut.
		MenuButtonCfg* addButton(const std::string& _name, const std::string& _text, const std::string& _iconPath = std::string(), MenuButtonCfg::ButtonAction _action = MenuButtonCfg::ButtonAction::NotifyOwner);

		//! @brief Creates and adds a separator.
		void addSeparator(void);

		//! @brief Searches for the menu button with the given name in this menu and all of its child menus.
		//! The menu keeps ownership of the button.
		MenuButtonCfg* findMenuButton(const std::string& _name) const;

		const std::list<MenuEntryCfg*>& getEntries(void) const { return m_childs; };

		//! @brief Returns true if this menu has no buttons (child menus do not count).
		bool isEmpty(void) const;

	private:
		void clear(void);

		std::list<MenuEntryCfg*> m_childs;

	};

}