/*
 *	File:		aRadioButtonWidget.h
 *	Package:	akWidgets
 *
 *  Created on: April 22, 2020
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

// Qt header
#include <qradiobutton.h>			// Base class
#include <qstring.h>				// QString

namespace ak {

	class UICORE_API_EXPORT aRadioButtonWidget : public QRadioButton, public aWidget
	{
		Q_OBJECT
	public:
		aRadioButtonWidget(QWidget * _parent = nullptr);

		aRadioButtonWidget(const QString & _text, QWidget * _parent = nullptr);

		virtual ~aRadioButtonWidget();

		// #############################################################################################################################
		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

		//! @brief Will set the objects color style
		//! @param _colorStyle The color style to set
		//! @throw ak::Exception if the provided color style is a nullptr or failed to repaint the object
		virtual void setColorStyle(
			aColorStyle *	_colorStyle
		) override;

	private:
		aRadioButtonWidget(const aRadioButtonWidget &) = delete;
		aRadioButtonWidget & operator = (const aRadioButtonWidget &) = delete;
	};
} // namespace ak
