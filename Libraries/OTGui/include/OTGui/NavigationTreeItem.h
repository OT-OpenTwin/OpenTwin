//! @file NavigationTreeItem.h
//! @author Alexander Kuester (alexk95)
//! @date January 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <list>
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT NavigationTreeItem : public ot::Serializable {
	public:
		NavigationTreeItem();
		NavigationTreeItem(const std::string& _text, ot::NavigationItemFlag _flags = ot::NoNavigationItemFlags);
		NavigationTreeItem(const std::string& _text, const std::string& _iconPath, ot::NavigationItemFlag _flags = ot::NoNavigationItemFlags);
		NavigationTreeItem(const std::string& _text, const std::string& _iconPath, const std::list<NavigationTreeItem>& _childItems, ot::NavigationItemFlag _flags = ot::NoNavigationItemFlags);
		NavigationTreeItem(const NavigationTreeItem& _other);
		virtual ~NavigationTreeItem();

		NavigationTreeItem& operator = (const NavigationTreeItem& _other);

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setText(const std::string& _text) { m_text = _text; };
		const std::string text(void) const { return m_text; };

		void setIconPath(const std::string& _iconPath) { m_iconPath = _iconPath; };
		const std::string& iconPath(void) const { return m_iconPath; };

		void addChildItem(const NavigationTreeItem& _item);
		void setChildItems(const std::list<NavigationTreeItem>& _items) { m_childs = _items; };
		const std::list<NavigationTreeItem>& childItems(void) const { return m_childs; };

		void setFlags(NavigationItemFlag _flags) { m_flags = _flags; };
		NavigationItemFlag flags(void) const { return m_flags; };

	private:
		std::string m_text;
		std::string m_iconPath;
		std::list<NavigationTreeItem> m_childs;
		NavigationItemFlag m_flags;

	};

}