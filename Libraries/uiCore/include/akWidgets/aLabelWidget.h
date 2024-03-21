/*
 *	File:		aLabelWidget.h
 *	Package:	akWidgets
 *
 *  Created on: October 27, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// Qt header
#include <qlabel.h>					// Base class
#include <qstring.h>				// QString

// AK header
#include <akWidgets/aWidget.h>
#include <akCore/globalDataTypes.h>

namespace ak {

	class UICORE_API_EXPORT aLabelWidget : public QLabel, public aWidget
	{
		Q_OBJECT
	public:
		aLabelWidget(QWidget * _parent = nullptr);

		aLabelWidget(const QString & _text, QWidget * _parent = nullptr);

		virtual ~aLabelWidget();

		// #############################################################################################################################
		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

	signals:
		void clicked();

	protected:
		bool event(QEvent *myEvent);

	private:
		aLabelWidget(const aLabelWidget &) = delete;
		aLabelWidget & operator = (const aLabelWidget &) = delete;
	};
} // namespace ak
