// @otlicense
// File: SelectionData.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

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

		void setViewHandlingFlag(ViewHandlingFlag _flag, bool _isSet = true) { m_viewHandlingFlags.setFlag(_flag, _isSet); };
		void setViewHandlingFlags(const ViewHandlingFlags& _flags) { m_viewHandlingFlags = _flags; };
		const ViewHandlingFlags& getViewHandlingFlags() const { return m_viewHandlingFlags; }
		bool isViewHandlingFlagSet(ViewHandlingFlag _flag) const { return (m_viewHandlingFlags & _flag) == _flag; };

	private:
		std::list<UID> m_selectedTreeItems;
		Qt::KeyboardModifiers m_keyboardModifiers;
		SelectionOrigin m_selectionOrigin;
		ViewHandlingFlags m_viewHandlingFlags;
	};

}