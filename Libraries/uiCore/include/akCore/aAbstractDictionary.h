/*
 *	File:		aAbstractDictionary.h
 *	Package:	akCore
 *
 *  Created on: August 11, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// uiCore header
#include <akCore/globalDataTypes.h>

// Qt header
#include <qstring.h>

namespace ak {
	class UICORE_API_EXPORT aAbstractDictionary {
	public:
		aAbstractDictionary() {}
		aAbstractDictionary(const aAbstractDictionary&) {}
		aAbstractDictionary& operator = (const aAbstractDictionary&) { return *this; }

		virtual ~aAbstractDictionary() {}

		//! @brief Will return the translation for the specified key
		virtual const QString& translate(const QString& _key) = 0;
		
	};
}