//! @file SelectionData.h
//! @author Alexander Kuester (alexk95)
//! @date August 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/GuiTypes.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>

// std header
#include <list>

namespace ot {

	class OT_WIDGETS_API_EXPORT SelectionData {
		OT_DECL_DEFCOPY(SelectionData)
		OT_DECL_DEFMOVE(SelectionData)
	public:
		SelectionData();
		~SelectionData();

		void setKeyboardModifiers(Qt::KeyboardModifiers _modifiers) { m_keyboardModifiers = _modifiers; };
		Qt::KeyboardModifiers getKeyboardModifiers() const { return m_keyboardModifiers; }
		bool hasKeyboardModifiers() const { return m_keyboardModifiers != Qt::NoModifier; }

		void setSelectedTreeItems(const std::list<UID>& _items) { m_selectedTreeItems = _items; };
		const std::list<UID>& getSelectedTreeItems() const { return m_selectedTreeItems; }

		void setSelectionOrigin(SelectionOrigin _origin) { m_selectionOrigin = _origin; };
		SelectionOrigin getSelectionOrigin() const { return m_selectionOrigin; }

	private:
		std::list<UID> m_selectedTreeItems;
		Qt::KeyboardModifiers m_keyboardModifiers;
		SelectionOrigin m_selectionOrigin;
	};

}