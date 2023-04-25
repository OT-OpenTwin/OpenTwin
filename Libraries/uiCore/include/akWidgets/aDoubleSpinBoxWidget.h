/*
 *	File:		aDoubleSpinBoxWidget.h
 *	Package:	akWidgets
 *
 *  Created on: June 17, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

 // AK header
#include <akWidgets/aWidget.h>
#include <akCore/globalDataTypes.h>

// QT header
#include <qspinbox.h>

class QMouseEvent;
class QKeyEvent;

namespace ak {

	class UICORE_API_EXPORT aDoubleSpinBoxWidget : public QDoubleSpinBox, public aWidget {
		Q_OBJECT
	public:
		aDoubleSpinBoxWidget(QWidget * _parent = (QWidget *) nullptr);
		aDoubleSpinBoxWidget(double _value, QWidget * _parent = (QWidget *) nullptr);
		virtual ~aDoubleSpinBoxWidget(void);

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

	protected:
		virtual void mouseReleaseEvent(QMouseEvent *event) override;
		virtual void keyReleaseEvent(QKeyEvent *event) override;
	};
}
