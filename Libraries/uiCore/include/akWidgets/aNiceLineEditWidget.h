// @otlicense
// File: aNiceLineEditWidget.h
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

// QT header
#include <qwidget.h>

class QHBoxLayout;
class QKeyEvent;

namespace ot {
	class Label;
}

namespace ak {
	
	class aLineEditWidget;
	
	class UICORE_API_EXPORT aNiceLineEditWidget : public QWidget, public aWidget {
		Q_OBJECT
		OT_DECL_NOCOPY(aNiceLineEditWidget)
		OT_DECL_NOMOVE(aNiceLineEditWidget)
		OT_DECL_NODEFAULT(aNiceLineEditWidget)
	public:
		aNiceLineEditWidget(const QString & _initialText, const QString & _infoLabelText, QWidget* _parent);
		virtual ~aNiceLineEditWidget();

		// #######################################################################################################

		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

		// #######################################################################################################

		void setText(const QString & _text);
		void setErrorState(bool _error);
		void setInfoLabelText(const QString & _text);

		//! @brief Will set wather the error state will have an impact on the foreground or the background
		//! @param _isForeground If true, the error state will change the text foreground color, otherwise back and foreground
		void setErrorStateIsForeground(bool _isForeground);

		QString text(void) const;
		QString infoLabelText(void) const;

	private Q_SLOTS:
		void slotCursorPositionChanged(int, int);
		void slotSelectionChanged();
		void slotKeyPressed(QKeyEvent * _event);
		void slotKeyReleased(QKeyEvent * _event);
		void slotEditingFinished(void);
		void slotTextChanged(const QString & _text);
		void slotReturnPressed();

	Q_SIGNALS:
		void cursorPositionChanged(int, int);
		void selectionChanged();
		void keyPressed(QKeyEvent *);
		void keyReleased(QKeyEvent *);
		void editingFinished();
		void textChanged(const QString & _text);
		void returnPressed();

	private:
		QHBoxLayout *			m_layout;
		aLineEditWidget *		m_lineEdit;
		ot::Label*				m_label;
	};
}
