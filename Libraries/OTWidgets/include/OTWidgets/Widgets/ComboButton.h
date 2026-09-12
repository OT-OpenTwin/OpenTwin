// @otlicense
// File: ComboButton.h
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
#include "OTWidgets/Widgets/PushButton.h"

class QMenu;
class QAction;

namespace ot {


	class OT_WIDGETS_API_EXPORT ComboButton : public PushButton {
		Q_OBJECT
		OT_DECL_NOCOPY(ComboButton)
		OT_DECL_NOMOVE(ComboButton)
		OT_DECL_NODEFAULT(ComboButton)
	public:
		explicit ComboButton(QWidget* _parent);
		explicit ComboButton(const QString& _text, QWidget* _parent);
		explicit ComboButton(const QString& _text, const QStringList& _items, QWidget* _parent);
		virtual ~ComboButton();

		virtual void mousePressEvent(QMouseEvent* _event) override;

		void clear();

		void addItem(const QString& _item);
		void addItem(const QString& _item, const QVariant& _userData);
		void setItems(const QStringList& _items);

		void setCurrentIndex(int _index);

		//! @brief Returns the index of the first item that matches the given text or -1 if no match is found.
		int getFirstMatchingIndex(const QString& _text, Qt::CaseSensitivity _caseSensitivity = Qt::CaseSensitivity::CaseSensitive) const;

		void setCurrentUserData(const QVariant& _userData) { m_currentUserData = _userData; };
		const QVariant& getCurrentUserData() const { return m_currentUserData; };

		//! @brief Sets the text of the combo button.
		void setText(const QString& _text);
		const QString& getText() const { return m_text; };

		//! @brief Sets the placeholder text for the combo button.
		//! If the combo button text is empty, the placeholder text will be displayed instead.
		//! @note This only has an effect when using ComboButton::setComboButtonText() to set the text of the button.
		void setPlaceholderText(const QString& _text);
		const QString& getPlaceholderText() const { return m_placeholderText; };

		void setPlaceholderFont(const QFont& _font);
		const QFont& getPlaceholderFont() const { return m_placeholderFont; };

		void setFont(const QFont& _font);
		const QFont& getFont() const { return m_defaultFont; };

	Q_SIGNALS:
		void selectedItemChanged();

	public Q_SLOTS:
		void updateDisplayText();

	private Q_SLOTS:
		void slotActionTriggered(QAction* _action);

	private:
		void ini();

		enum class State
		{
			None = 0 << 0,
			PlaceholderShown = 1 << 0
		};
		typedef ot::Flags<State> StateFlags;
		OT_ADD_FRIEND_FLAG_FUNCTIONS(State, StateFlags)

		StateFlags m_state;

		QMenu* m_menu;
		QVariant m_currentUserData;
		QString m_placeholderText;
		QString m_text;
		QFont m_placeholderFont;
		QFont m_defaultFont;
	};

}
