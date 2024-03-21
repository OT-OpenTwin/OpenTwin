/*
 *	File:		aCheckBoxWidget.h
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
#include <qcheckbox.h>				// base class
#include <qstring.h>				// QString

// AK header
#include <akWidgets/aWidget.h>
#include <akCore/globalDataTypes.h>

// Forward declaration
class QKeyEvent;

namespace ak {

	//! @brief This class combines the functionallity of a QCheckBox and a ak::object
	class UICORE_API_EXPORT aCheckBoxWidget : public QCheckBox, public aWidget
	{
		Q_OBJECT
	public:
		//! @brief Constructor
		//! @param _parent The parent QWidget for this CheckBox
		aCheckBoxWidget(
			QWidget *								_parent = (QWidget *) nullptr
		);

		//! @brief Constructor
		//! @param _text The initial text of the CheckBox
		//! @param _parent The parent QWidget for this CheckBox
		aCheckBoxWidget(
			const QString &							_text,
			QWidget *								_parent = (QWidget *) nullptr
		);

		//! @brief Deconstructor
		virtual ~aCheckBoxWidget();

		// #######################################################################################################

		// Event handling

		//! @brief Emits a key pressend signal a key is pressed
		virtual void keyPressEvent(
			QKeyEvent *								_event
		) override;

		//! @brief Emits a key released signal a key is released
		virtual void keyReleaseEvent(
			QKeyEvent *								_event
		) override;

		// #######################################################################################################

		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

		// #######################################################################################################

		void setPaintBackground(bool _paint);

	signals:
		void keyPressed(QKeyEvent *);
		void keyReleased(QKeyEvent *);

	private:
		bool	m_paintBackground;
	};
} // namespace ak
