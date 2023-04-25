/*
 *	File:		aTextEditWidget.h
 *	Package:	akWidgets
 *
 *  Created on: March 19, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// Qt header
#include <qtextedit.h>				// base class
#include <qpoint.h>					// QPoint

// AK header
#include <akCore/globalDataTypes.h>
#include <akCore/akCore.h>
#include <akWidgets/aWidget.h>

// Forward declaration
class QKeyEvent;
class QMenu;
class QFocusEvent;
namespace ak {
	// Forward declaration
	class aDockWidget;
	class aAction;
	class aContextMenuItem;

	//! @brief This class combines the functionallity of a QTextEdit and a ak::ui::core::aWidget
	class UICORE_API_EXPORT aTextEditWidget : public QTextEdit, public aWidget
	{
		Q_OBJECT
	public:
		//! @brief Constructor
		//! @param _parent The parent QWidget for this textEdit
		aTextEditWidget(QWidget * _parent = (QWidget *) nullptr);

		//! @brief Constructor
		//! @param _text The initial text for this text edit
		//! @param _parent The parent QWidget for this textEdit
		aTextEditWidget(const QString & _text, QWidget * _parent = (QWidget *) nullptr);

		//! @brief Deconstructor
		virtual ~aTextEditWidget();

		// #######################################################################################################
		// Event handling

		//! @brief Emits a returnPressed signal if the return key is pressed
		virtual void keyPressEvent(QKeyEvent * _event) override;

		virtual void keyReleaseEvent(QKeyEvent *event) override;

		virtual void focusInEvent(QFocusEvent * _event) override;

		virtual void focusOutEvent(QFocusEvent * _event) override;

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

		// Context menu

		//! @brief Will add a context menu item and return its ID
		//! @param _item The item to add
		ID addContextMenuItem(
			aContextMenuItem *			_item
		);

		//! @brief Will add a sparator at the context menu
		void addContextMenuSeparator(void);

		//! @brief Will remove all context menu items from the context menu
		void clearContextMenu(void);

		// #######################################################################################################

		//! @brief Will set the auto scroll to bottom option.
		//! @param _autoScroll If true, the object will automatically scroll to the bottom on change
		void setAutoScrollToBottom(
			bool							_autoScroll
		);

		//! @brief Returns the auto scroll to bottom option
		bool autoScrollToBottom(void) const;

		//! @brief Will perform the auto scroll to bottom
		void performAutoScrollToBottom(void);

		void setMaxTextLength(int _length) { m_maxLength = _length; }

		int maxTextLength(void) const { return m_maxLength; }

	signals:
		void keyPressed(QKeyEvent *);
		void keyReleased(QKeyEvent *);
		void contextMenuItemClicked(ak::ID);
		void focused(void);
		void focusLost(void);

	private slots:
		void slotChanged();
		void slotCustomMenuRequested(const QPoint & _pos);
		void slotContextMenuItemClicked();

	private:
		bool							m_autoScrollToBottom;		//! If true, the textbox will automatically scroll down on text change
		QMenu *							m_contextMenu;
		ID								m_currentContextMenuItemId;
		int								m_maxLength;
		bool							m_controlIsPressed;

		std::vector<aContextMenuItem *>	m_contextMenuItems;

		// Initialize the textEdit
		void ini(void);

		aTextEditWidget(const aTextEditWidget &) = delete;
		aTextEditWidget & operator = (const aTextEditWidget &) = delete;

	};
}