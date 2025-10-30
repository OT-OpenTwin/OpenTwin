// @otlicense
// File: aToolButtonWidget.h
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

 // AK header
#include <akCore/globalDataTypes.h>
#include <akWidgets/aWidget.h>

// Qt header
#include <qtoolbutton.h>				// Base class
#include <qstring.h>					// QString
#include <qicon.h>						// QIcon
#include <qpoint.h>

class QMenu;

namespace ak {

	class aAction;

	class UICORE_API_EXPORT aToolButtonWidget : public QToolButton, public aWidget {
		Q_OBJECT
	public:
		aToolButtonWidget();

		aToolButtonWidget(
			const QString &				_text
		);

		aToolButtonWidget(
			const QIcon &				_icon,
			const QString &				_text
		);

		virtual ~aToolButtonWidget();

		// #######################################################################################################

		// Event handling

		//! @brief Emits a key pressend signal a key is pressed
		virtual void keyPressEvent(QKeyEvent * _event) override;

		//! @brief Emits a key released signal a key is released
		virtual void keyReleaseEvent(QKeyEvent * _event) override;

		// #######################################################################################################

		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override { return this; }

		// #######################################################################################################

		//! @brief Will return the action of this tool button
		aAction * getAction() const { return m_action; }

		//! @brief Will set the toolTip and WhatsThis of this toolButton and its action
		//! @param _text The text to set
		void SetToolTip(
			const QString &						_text
		);

		//! @brief Will return the current toolTip of this toolButton
		QString ToolTip(void) const;

	Q_SIGNALS:
		void btnClicked();
		void keyPressed(QKeyEvent *);
		void keyReleased(QKeyEvent *);

	private Q_SLOTS:
		void slotClicked();

	private:

		//! @brief Initializes the components of this toolButton
		void ini(void);

		aAction *						m_action;

	};
}
