#pragma once

// Foundation header
#include "OTServiceFoundation/FoundationAPIExport.h"

// C++ header
#include <string>

namespace ot {
	
	class OT_SERVICEFOUNDATION_API_EXPORT AbstractUiNotifier {
	public:
		AbstractUiNotifier() {}
		virtual ~AbstractUiNotifier() {}

		//! @brief Will be called when a key sequence was activated in the ui
		//! @param _keySequence The key sequence that was activated
		virtual void shortcutActivated(const std::string& _keySquence) = 0;

		//! @brief Will be called when an item of the context menu was clicked
		//! @param _menuName The name of the context menu where the item was clicked
		//! @param _itemName The name of the context menu item that was clicked
		virtual void contextMenuItemClicked(const std::string& _menuName, const std::string& _itemName) = 0;

		//! @brief Will be called when the checked state of the item was changed
		//! @param _menuName The name of the context menu where the item was changed
		//! @param _itemName The name of the context menu item that was changed
		//! @param _isChecked True if the item is currently checked
		virtual void contextMenuItemCheckedChanged(const std::string& _menuName, const std::string& _itemName, bool _isChecked) = 0;

	private:
		AbstractUiNotifier(AbstractUiNotifier&) = delete;
		AbstractUiNotifier& operator = (AbstractUiNotifier&) = delete;
	};

}