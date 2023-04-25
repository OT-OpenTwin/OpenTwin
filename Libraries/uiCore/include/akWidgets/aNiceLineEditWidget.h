/*
 *	File:		aNiceLineEditWidget.h
 *	Package:	akWidgets
 *
 *  Created on: April 02, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// AK header
#include <akCore/globalDataTypes.h>
#include <akWidgets/aWidget.h>

// QT header
#include <qwidget.h>

class QHBoxLayout;
class QKeyEvent;

namespace ak {
	class aLineEditWidget;
	class aLabelWidget;

	class UICORE_API_EXPORT aNiceLineEditWidget : public QWidget, public aWidget {
		Q_OBJECT
	public:
		aNiceLineEditWidget(const QString & _initialText, const QString & _infoLabelText);
		virtual ~aNiceLineEditWidget();

		// #######################################################################################################

		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

		//! @brief Will set the objects color style
		//! @param _colorStyle The color style to set
		//! @throw ak::Exception if the provided color style is a nullptr or failed to repaint the object
		virtual void setColorStyle(
			aColorStyle *			_colorStyle
		) override;

		// #######################################################################################################

		void setText(const QString & _text);
		void setErrorState(bool _error);
		void setInfoLabelText(const QString & _text);

		//! @brief Will set wather the error state will have an impact on the foreground or the background
		//! @param _isForeground If true, the error state will change the text foreground color, otherwise back and foreground
		void setErrorStateIsForeground(bool _isForeground);

		QString text(void) const;
		QString infoLabelText(void) const;

	private slots:
		void slotCursorPositionChanged(int, int);
		void slotSelectionChanged();
		void slotKeyPressed(QKeyEvent * _event);
		void slotKeyReleased(QKeyEvent * _event);
		void slotEditingFinished(void);
		void slotTextChanged(const QString & _text);
		void slotReturnPressed();

	signals:
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
		aLabelWidget *			m_label;
	};
}