/*
 *	File:		aWidget.h
 *	Package:	akWidgets
 *
 *  Created on: July 26, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// AK header
#include <akCore/globalDataTypes.h>
#include <akCore/akCore.h>
#include <akGui/aPaintable.h>

// Forward declaration
class QWidget;

namespace ak {

	// Forward declaration
	class aColorStyle;

	//! This class provides a interface that represents a widget.
	//! Every class derived from this class must be able to provide a QWidget that is representing it
	class UICORE_API_EXPORT aWidget : public aPaintable {
	public:
		//! @brief Constructor
		//! @param _UID The initial UID of this object
		//! @param _colorStyle The initial color style used in this paintable
		aWidget(
			objectType			_type = otNone,
			aColorStyle *		_colorStyle = nullptr,
			UID					_UID = invalidUID
		);

		//! @brief Deconstructor
		virtual ~aWidget();

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) = 0;

		//! @brief Returns true the object is derived from aWidget
		virtual bool isWidgetType(void) const override;

	private:
		// Block copy constructor
		aWidget(const aWidget & _other) = delete;

		// Block assignment operator
		aWidget & operator = (const aWidget & _other) = delete;
	};
} // namespace ak