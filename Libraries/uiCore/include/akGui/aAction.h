// @otlicense
// File: aAction.h
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

// Qt header
#include <qaction.h>					// base class
#include <qstring.h>					// QString
#include <qicon.h>						// QIcon
#include <qtoolbutton.h>				// ToolButtonPopupMode

// AK header
#include <akCore/globalDataTypes.h>
#include <akCore/aObject.h>

namespace ak {

	//! @brief This class combines the functionallity of a QAction and a ak::ui::core::aPaintable
	class UICORE_API_EXPORT aAction : public QAction, public aObject
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

		// #######################################################################################################

		//! @brief Retuns the popup mode this Action is using
		QToolButton::ToolButtonPopupMode popupMode(void) { return m_popupMode; }

	private:
		QToolButton::ToolButtonPopupMode			m_popupMode;		//! The popup Mode of this Action
	};
} // namespace ak
