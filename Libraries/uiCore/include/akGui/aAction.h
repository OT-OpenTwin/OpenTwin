/*
 *	File:		aAction.h
 *	Package:	akGui
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
#include <qaction.h>					// base class
#include <qstring.h>					// QString
#include <qicon.h>						// QIcon
#include <qtoolbutton.h>				// ToolButtonPopupMode

// AK header
#include <akCore/globalDataTypes.h>
#include <akGui/aPaintable.h>

namespace ak {

	//! @brief This class combines the functionallity of a QAction and a ak::ui::core::aPaintable
	class UICORE_API_EXPORT aAction : public QAction, public aPaintable
	{
		Q_OBJECT
	public:

		//! @brief Constructor
		//! @param _popupMode The popup mode used for this Action
		//! @param _parent The parent QWidget for this Action
		aAction(
			QToolButton::ToolButtonPopupMode	_popupMode = QToolButton::ToolButtonPopupMode::InstantPopup,
			QObject *							_parent = (QObject *) nullptr
		);

		//! @brief Constructor
		//! @param _text The initial text for this Action
		//! @param _popupMode The popup mode used for this Action
		//! @param _parent The parent QWidget for this Action
		aAction(
			const QString &						_text,
			QToolButton::ToolButtonPopupMode	_popupMode = QToolButton::ToolButtonPopupMode::InstantPopup,
			QObject *							_parent = (QObject *) nullptr
		);

		//! @brief Constructor
		//! @param _icon The initial icon used for this Action
		//! @param _text The initial text for this Action
		//! @param _popupMode The popup mode used for this Action
		//! @param _parent The parent QWidget for this Action
		aAction(
			const QIcon &						_icon,
			const QString &						_text,
			QToolButton::ToolButtonPopupMode	_popupMode = QToolButton::ToolButtonPopupMode::InstantPopup,
			QObject *							_parent = (QObject *) nullptr
		);

		//! @brief Deconstructor
		virtual ~aAction();

		//! @brief Will set the objects color style
		//! @param _colorStyle The color style to set
		//! @throw ak::Exception if the provided color style is a nullptr or failed to repaint the object
		virtual void setColorStyle(
			aColorStyle *					_colorStyle
		) override;

		// #######################################################################################################

		//! @brief Retuns the popup mode this Action is using
		QToolButton::ToolButtonPopupMode popupMode(void) { return m_popupMode; }

	private:
		QToolButton::ToolButtonPopupMode			m_popupMode;		//! The popup Mode of this Action
	};
} // namespace ak
