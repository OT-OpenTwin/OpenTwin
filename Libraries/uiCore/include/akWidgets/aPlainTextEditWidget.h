/*
 *	File:		aPlainTextEditWidget.h
 *	Package:	akWidgets
 *
 *  Created on: October 14, 2022
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// uiCore header
#include <akCore/globalDataTypes.h>
#include <akWidgets/aWidget.h>

// Qt header
#include <qplaintextedit.h>

class QKeyEvent;
class QFocusEvent;

namespace ak {
	class UICORE_API_EXPORT aPlainTextEditWidget : public QPlainTextEdit, public aWidget {
		Q_OBJECT
	public:
		aPlainTextEditWidget(QWidget * _parent = (QWidget *) nullptr);
		aPlainTextEditWidget(const QString& _text, QWidget * _parent = (QWidget *) nullptr);
		virtual ~aPlainTextEditWidget();

		//! @brief Will return the widget
		virtual QWidget * widget(void) override;

		//! @brief Emits a returnPressed signal if the return key is pressed
		virtual void keyPressEvent(QKeyEvent * _event) override;

		virtual void keyReleaseEvent(QKeyEvent * _event) override;

		virtual void focusInEvent(QFocusEvent * _event) override;

		virtual void focusOutEvent(QFocusEvent * _event) override;

	Q_SIGNALS:
		void keyPressed(QKeyEvent * _event);
		void keyReleased(QKeyEvent * _event);
		void focused(void);
		void focusLost(void);

	private:
		bool	m_controlIsPressed;
	};
}
