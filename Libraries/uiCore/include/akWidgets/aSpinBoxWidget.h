/*
 *	File:		aSpinBoxWidget.h
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

	class UICORE_API_EXPORT aSpinBoxWidget : public QSpinBox, public aWidget {
		Q_OBJECT
	public:
		aSpinBoxWidget(QWidget * _parent = (QWidget *) nullptr);
		aSpinBoxWidget(int _value, QWidget * _parent = (QWidget *) nullptr);
		virtual ~aSpinBoxWidget(void);

		// #############################################################################################################################
		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

		//! @brief Will set the step length for the spinBox
		void SetStepLength(int _stepLength);

		int StepLength(void) const { return m_stepLength; }

	private slots:
		void slotValueChanged(int _v);

	signals:
		void aValueChanged(int _value);

	protected:
		virtual void mouseReleaseEvent(QMouseEvent *event) override;
		virtual void keyReleaseEvent(QKeyEvent *event) override;

	protected:
		int		m_stepLength;
		int		m_value;
	};
}
