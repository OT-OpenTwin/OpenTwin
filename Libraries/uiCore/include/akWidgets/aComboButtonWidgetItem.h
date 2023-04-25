/*
 *	File:		aComboButtonWidgetItem.h
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
#include <qaction.h>				// base class
#include <qicon.h>					// QIcon
#include <qstring.h>				// QString

// AK header
#include <akCore/globalDataTypes.h>
#include <akCore/aObject.h>

namespace ak {
	//! @brief This class represents a menu item of a combo button
	class UICORE_API_EXPORT aComboButtonWidgetItem : public QAction, public aObject
	{
		Q_OBJECT
	public:

		//! @brief Constructor
		//! @param _parent The parent QWidget for this combo button item
		aComboButtonWidgetItem(
			QObject * _parent = (QObject *) nullptr);

		//! @brief Constructor
		//! @param _text The initial text of this combo button item
		//! @param _parent The parent QWidget for this combo button item
		aComboButtonWidgetItem(
			const QString & _text,
			QObject * _parent = (QObject *) nullptr);

		//! @brief Constructor
		//! @param _icon The initial icon of this combo button item
		//! @param _text The initial text of this combo button item
		//! @param _parent The parent QWidget for this combo button item
		aComboButtonWidgetItem(
			const QIcon & _icon,
			const QString & _text,
			QObject * _parent = (QObject *) nullptr);

		//! @brief Copy constructor
		//! @param _other The other combo button item
		aComboButtonWidgetItem(
			const aComboButtonWidgetItem &	_other
		);

		//! @brief Assignment operator
		//! @param _other The other combo button item
		aComboButtonWidgetItem & operator = (const aComboButtonWidgetItem & _other);

		//! @brief Deconstructor
		virtual ~aComboButtonWidgetItem();

	};
}
