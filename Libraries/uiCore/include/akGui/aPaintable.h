/*
 *	File:		aPaintable.h
 *	Package:	akGui
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
#include <akCore/aObject.h>

namespace ak {
	// Forward declaration
	class aColorStyle;

	//! This class represents a object that can be painted.
	//! More specific a object where the color style may be applied to, either by using a stylesheet or simply by using setBack- or setForeColor
	class UICORE_API_EXPORT aPaintable : public aObject {
	public:

		//! @brief Constructor
		//! @param _UID The initial UID of this object
		//! @param _colorStyle The initial color style used in this paintable
		aPaintable(
			objectType				_type = otNone,
			aColorStyle *			_colorStyle = nullptr,
			UID						_UID = invalidUID
		);

		//! @brief Deconstructor
		virtual ~aPaintable();

		//! @brief Will set the objects color style
		//! @param _colorStyle The color style to set
		//! @throw ak::Exception if the provided color style is a nullptr or failed to repaint the object
		virtual void setColorStyle(
			aColorStyle *			_colorStyle
		) = 0;

		//! @brief Returns true because the object is derived from a aPaintable
		virtual bool isPaintableType(void) const override;

	protected:
		aColorStyle *			m_colorStyle;			//! The current objects color style

	private:
		// Block copy constructor
		aPaintable(const aPaintable & _other) = delete;

		// Block assignment operator
		aPaintable & operator = (const aPaintable & _other) = delete;
	};
} // namespace ak
