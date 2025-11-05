// @otlicense
// File: aLineEditWidget.h
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
#include <qlineedit.h>				// Base class

// Forward declaration
class QKeyEvent;
class QFocusEvent;

namespace ak {

	//! @brief This class combines the functionallity of a QTextEdit and a ak::ui::core::aWidget
	class UICORE_API_EXPORT aLineEditWidget : public QLineEdit, public aWidget
	{
		Q_OBJECT
		OT_DECL_NOCOPY(aLineEditWidget)
		OT_DECL_NOMOVE(aLineEditWidget)
		OT_DECL_NODEFAULT(aLineEditWidget)
	public:
		//! @brief Constructor
		//! @param _parent The parent QWidget for this textEdit
		explicit aLineEditWidget(QWidget * _parent);

		//! @brief Constructor
		//! @param _text The initial text for this text edit
		//! @param _parent The parent QWidget for this textEdit
		explicit aLineEditWidget(const QString & _text, QWidget* _parent);

		//! @brief Deconstructor
		virtual ~aLineEditWidget();

		// #######################################################################################################
		
		// Event handling

		virtual bool eventFilter(QObject * _obj, QEvent * _event) override;

		//! @brief Emits a returnPressed signal if the return key is pressed
		virtual void keyPressEvent(QKeyEvent * _event) override;

		virtual void keyReleaseEvent(QKeyEvent *event) override;

		virtual void focusInEvent(QFocusEvent * _event) override;
		virtual void focusOutEvent(QFocusEvent * _event) override;

		// #######################################################################################################

		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

		// #######################################################################################################

		// Setter

		void setErrorState(bool _error);

		//! @brief Will set wather the error state will have an impact on the foreground or the background
		//! @param _isForeground If true, the error state will change the text foreground color, otherwise back and foreground
		void setErrorStateIsForeground(bool _isForeground);

		void setIgnoreTabulator(bool _ignore) { m_ignoreTabulator = _ignore; }

		// #######################################################################################################

		// Getter

		bool ignoreTabulator(void) const { return m_ignoreTabulator; }

	Q_SIGNALS:
		void keyPressed(QKeyEvent *);
		void keyReleased(QKeyEvent *);
		void finishedChanges();
		//void returnPressed(void);
		void tabPressed();
		void focused(void);
		void focusLost(void);

	private Q_SLOTS:
		void slotEditingFinished();

	private:
		bool					m_isError;
		bool					m_errorIsForeground;
		bool					m_ignoreTabulator;
		QString					m_text;
	};
}
