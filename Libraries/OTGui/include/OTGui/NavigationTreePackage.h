// @otlicense

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/NavigationTreeItem.h"

// std header
#include <list>

namespace ot {

	class OT_GUI_API_EXPORT NavigationTreePackage : public ot::Serializable {
	public:
		enum NavigationTreePackageFlag {
			NoFlags = 0x00,
			ItemsDefaultExpanded = 0x01 //! @brief When opening the dialog the items are expanded
		};
		typedef Flags<NavigationTreePackageFlag> NavigationTreePackageFlags;

		static std::string toString(NavigationTreePackageFlag _flag);
		static NavigationTreePackageFlag stringToFlag(const std::string& _flag);

		NavigationTreePackage();
		virtual ~NavigationTreePackage();

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setFlag(NavigationTreePackageFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };
		void setFlags(NavigationTreePackageFlags _flags) { m_flags = _flags; };
		NavigationTreePackageFlags flags(void) const { return m_flags; };

		void setRootItems(const std::list<NavigationTreeItem>& _items) { m_rootItems = _items; };
		void addRootItem(const NavigationTreeItem& _item);
		const std::list<NavigationTreeItem>& rootItems(void) const { return m_rootItems; };

		//! @brief Will merge items with the same name into one
		//! @param _mergeAllChilds If true merge all child items aswell
		void mergeItems(bool _mergeAllChilds = true);

	private:
		NavigationTreePackageFlags m_flags;
		std::list<NavigationTreeItem> m_rootItems;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::NavigationTreePackage::NavigationTreePackageFlag)